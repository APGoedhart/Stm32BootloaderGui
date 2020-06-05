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

public:
    Command();
    Command(QString name);

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
    void stepFailed();

    // factory methods
    static CmdPtr resetIntoBootLoader(CommandSequence *seq);
    static CmdPtr resetIntoRunMode(CommandSequence *seq);
    static CmdPtr syncBaud();
    static CmdPtr eraseFlash();
    static CmdPtr writeData(uint32_t address, const QByteArray &dataPage);
    static CmdPtr verifyData(uint32_t address, const QByteArray &dataPage);

    void executeNextStep();
};

#endif // COMMAND_H
