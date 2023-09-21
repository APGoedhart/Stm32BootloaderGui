#ifndef COMMAND_H
#define COMMAND_H

#include <memory>
#include <QObject>
#include <QList>
#include <QString>
#include <QByteArray>

class CommandSequence;
class CommandStep;

class Command {
    using CmdPtr = std::shared_ptr<Command>;
    using StepPtr = std::shared_ptr<CommandStep>;

    QString _name;
    QList<StepPtr> _steps;
    CommandSequence *_owner;
    int _currentStep;
    int retries;
    int maxRetries;

public:
    Command();
    Command(QString name);
    Command(QString name, uint32_t retriesAllowed);

    QString name();
    void setOwner(CommandSequence *owner);
    void addStep(StepPtr testStep);

    virtual void execute();
    virtual void rxData( const QByteArray &data);
    virtual void timerExpired();

    // Services for steps
    void txData(const QByteArray &data);
    void scheduleTimeout(int milliseconds);


    // notifications from steps
    void stepComplete();
    virtual void stepFailed();

    // factory methods
    static CmdPtr resetIntoCustomBootLoader(CommandSequence *seq);
    static CmdPtr resetIntoChipBootLoader(CommandSequence *seq);
    static CmdPtr resetIntoRunMode(CommandSequence *seq);
    static CmdPtr syncBaud();
    static CmdPtr enableDebug(CommandSequence *seq);
    static CmdPtr bootloadSlave(CommandSequence *seq);
    static CmdPtr eraseFlash();
    static CmdPtr writeData(uint32_t address, const QByteArray &dataPage);
    static CmdPtr verifyData(uint32_t address, const QByteArray &dataPage);
    static CmdPtr programCrc();

    void executeNextStep();
};

#endif // COMMAND_H
