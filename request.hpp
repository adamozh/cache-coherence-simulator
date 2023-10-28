#pragma once

class Request {
  private:
    bool done = false;
    unsigned int countdown = 0;

  public:
    bool isDone() { return done; };
    void decrement() { countdown--; }
};
