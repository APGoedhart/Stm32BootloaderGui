#include "commands/command.h"
#include "commands/commandstep.h"
#include "commands/commandSequence.h"

Command::Command():
    _name("Not Set"),
    _steps(),
    _owner(nullptr),
    _currentStep(0){
}
Command::Command(QString name):
    _name(name),
    _steps(),
    _owner(nullptr),
    _currentStep(0){
}

QString Command::name(){
    return _name;
}

void Command::setOwner(CommandSequence *owner){
  _owner = owner;
}

void Command::addStep(Command::StepPtr testStep){
  _steps.append(testStep);
  testStep->setCommand(this);
}

void Command::executeNextStep()
{
    if(_currentStep < _steps.size()){
      _steps[_currentStep]->execute();
    } else {
      if( _owner != nullptr){
        _owner->CommandCompletedOkay();
      }
    }
}

void Command::execute(){
  _currentStep = 0;
  executeNextStep();
}

void Command::rxData(const QByteArray &data){
    if(_currentStep < _steps.size()){
      _steps[_currentStep]->rxData(data);
    }
}

void Command::timerExpired(){
    _owner->stopTimer();
    if(_currentStep < _steps.size()){
      _steps[_currentStep]->timerExpired();
    }
}

void Command::txData(const QByteArray &data)
{
    _owner->txData(data);
}

void Command::scheduleTimeout(int milliseconds)
{
    _owner->scheduleTimeout(milliseconds);
}

void Command::stepComplete(){
   _currentStep++;
  executeNextStep();
}

void Command::stepFailed(){
  if( _owner != nullptr){
    _owner->CommandFailed();
  }
}



Command::CmdPtr Command::resetIntoBootLoader(CommandSequence *seq){
    auto pullBootPinHighResetHigh = [seq](Command *){
      seq->dtr(false);
      seq->rts(true);
    };
    auto pullResetLow = [seq](Command *){
      seq->dtr(false);
    };
    auto pullResetHigh = [seq](Command *){
      seq->dtr(true);
    };


  auto cmd = std::make_shared<Command>("Reset into Bootloader");
  cmd->addStep(CommandStep::timedStep(pullBootPinHighResetHigh, 500));
  cmd->addStep(CommandStep::timedStep(pullResetLow, 500));
  cmd->addStep(CommandStep::timedStep(pullResetHigh, 500));
  return cmd;
}


Command::CmdPtr Command::resetIntoRunMode(CommandSequence *seq){
    auto pullBootPinLow = [seq](Command *){
      seq->rts(false);
    };
    auto pullResetLow = [seq](Command *){
      seq->dtr(false);
    };
    auto pullResetHigh = [seq](Command *){
      seq->dtr(true);
    };

  auto cmd = std::make_shared<Command>("Reset into Run");
  cmd->addStep(CommandStep::timedStep(pullResetLow, 1000));
  cmd->addStep(CommandStep::timedStep(pullBootPinLow, 1000));
  cmd->addStep(CommandStep::timedStep(pullResetHigh, 1000));
  return cmd;
}


Command::CmdPtr Command::syncBaud(){
  auto cmd = std::make_shared<Command>("Syncing ......");
  cmd->addStep(CommandStep::txBaudSync());
  return cmd;
}

Command::CmdPtr Command::eraseFlash(){
    auto cmd = std::make_shared<Command>("Erase Flash ");
    cmd->addStep(CommandStep::txCmd(0x43));
    cmd->addStep(CommandStep::txCmd(0xFF));
    return cmd;
}

Command::CmdPtr Command::writeData(uint32_t address, const QByteArray &dataPage){
  QByteArray addr;
  addr.append( (address>>24) &0xFF);
  addr.append( (address>>16) &0xFF);
  addr.append( (address>>8) &0xFF);
  addr.append(address &0xFF);

  auto description = QString("Write data address [%1]").arg(address,8,16);
  auto cmd = std::make_shared<Command>(description);
  cmd->addStep(CommandStep::txCmd(0x31));
  cmd->addStep(CommandStep::txFixedData(addr));
  cmd->addStep(CommandStep::txVariableData(dataPage));
  return cmd;
}

Command::CmdPtr Command::verifyData(uint32_t address, const QByteArray &dataPage){
  QByteArray addr;
  addr.append( (address>>24) &0xFF);
  addr.append( (address>>16) &0xFF);
  addr.append( (address>>8) &0xFF);
  addr.append(address &0xFF);

  auto description = QString("Verify data address [%1]").arg(address,8,16);
  auto cmd = std::make_shared<Command>(description);
  cmd->addStep(CommandStep::txCmd(0x11));
  cmd->addStep(CommandStep::txFixedData(addr));
  cmd->addStep(std::make_shared<RxVerifyData>(dataPage));
  return cmd;
}
