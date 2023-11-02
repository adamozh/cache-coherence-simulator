#pragma once

class Request {
  private:
    bool done = false;
    bool isModified;
    unsigned int countdown = 0;
    unsigned int pid;

  public:
    Request(int pid);
    bool isDone();
    void decrement();
    void setCountdown(unsigned int countdown);
};
