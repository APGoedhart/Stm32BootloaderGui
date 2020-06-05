#ifndef COMMANDSTEP_H
#define COMMANDSTEP_H

#include <QString>
#include <cstdint>
#include <memory>


class Command;

class CommandStep {
  public:
    static constexpr uint8_t ACK = 0x79;
    static constexpr uint8_t NACK = 0x1F;


    using StepPtr = std::shared_ptr<CommandStep>;
    CommandStep() :_owner(nullptr){}
    CommandStep(const CommandStep &other) = default;
    CommandStep &operator=(const CommandStep &other) = default;
    virtual ~CommandStep()=default;

    virtual void execute()=0;
    virtual void rxData(const QByteArray &data)=0;
    virtual void timerExpired()=0;

    void setCommand(Command *owner);
    Command *cmd();

    // Steps
    // -----
    /**
     * @brief txBaudSync send a sync byte and wait for an ACK.
     * @return
     */
    static StepPtr txBaudSync();

    /**
     * @brief txCmd generate a test step that sends a command and waits for an ack/NACK
     * @param cmdVal The command id
     * @return A step preforming this.
     */
    static StepPtr txCmd(uint8_t cmdVal);

    /**
     * @brief txFixedData transmit a packet with a fixed length to the boot loader.
     * @param data the data in the packet
     * @return a step that sends a fixed length packet with check byte at the end and waits for an ACK
     */
    static StepPtr txFixedData(const QByteArray &data);

    /**
     * @brief txVariableData transmit a packet with variable length to the boot loader.
     * @param data the data in the packet minus the length byte at the front
     * @return a step that sends a variable lenght packet with check byte at the end and waits for an ACK.
     */
    static StepPtr txVariableData(const QByteArray &data);

    /**
     * Execute step and then wait for a given number of millliseconds before finishing
     */
    template <typename FN>
    static StepPtr timedStep(FN &exec, int timeout);

    /**
     * @brief txVariableData transmit a packet with variable length to the boot loader.
     * @param data the data in the packet minus the length byte at the front
     * @return a step that sends a variable lenght packet with check byte at the end and waits for an ACK.
     */
    static StepPtr verifyData(const QByteArray &data);

    /**
     * helper function to create a QbyteArray from a byte list.
     */
    static QByteArray createArray(std::initializer_list<uint8_t>);


    /**
     * @brief appendCheckSum Append a checksum onto the end of the data packet
     * @param data the data for the packet
     * @return the data with an XOR checksum appended onto the end.
     */
    static QByteArray appendCheckSum(const QByteArray &data);

protected:
    Command *_owner;
};


template <typename FN1, typename FN2, typename FN3>
class LambdaStep :public CommandStep {
  FN1 _execute;
  FN2 _rxData;
  FN3 _timerExpired;

public:
  LambdaStep(const FN1 &exec, const FN2 &rxData, const FN3 &timerExpired):
      _execute(exec),
      _rxData(rxData),
      _timerExpired(timerExpired){
  }

  ~LambdaStep() override = default;

  void execute() override{
      _execute(cmd());
  }
  void rxData(const QByteArray &data) override{
    _rxData(cmd(), data);
  };

  void timerExpired() override{
      _timerExpired(cmd());
  }
};


class RxVerifyData :public CommandStep {
  QByteArray _expected;
  int  _rxIndex;

public:
  RxVerifyData(QByteArray expected):
      CommandStep(),
      _expected(),
      _rxIndex(0){
   _expected.append(ACK);
   _expected.append(expected);
  }

  ~RxVerifyData() override = default;

  void execute() override;
  void rxData(const QByteArray &data) override;
  void timerExpired() override{}

};

#include <commands/command.h>

template<typename FN>
CommandStep::StepPtr CommandStep::timedStep(FN &exec, int milliseconds)
{
    auto execWrapper = [milliseconds, exec](Command *cmd){
        exec(cmd);
        cmd->scheduleTimeout(milliseconds);
    };
    auto rxData = [] (Command *, const QByteArray &){};
    auto timerExpired = [] (Command *cmd){
        cmd->stepComplete();
    };
    return std::make_shared<LambdaStep<decltype(execWrapper), decltype(rxData), decltype(timerExpired)>>(execWrapper, rxData, timerExpired);
}

#endif // COMMANDSTEP_H
