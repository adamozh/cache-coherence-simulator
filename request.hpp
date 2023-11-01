#pragma once

class Request {
  private:
    bool done = false;
    unsigned int countdown = 0;
    unsigned int address = Null;
  public:
    bool isDone() { return done; };
    void decrement() { countdown--; }
};
