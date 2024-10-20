Modbus scanner library based on boost::coroutine2.

Modbus scanner takes stream of bytes as input and produces callbacks with Protocol Data Units (PDUs) on its output.
It looks very easy on the first sight but current portion of bytes in input may contain only part of PDU or several PDUs. As a result of this scanner has to "remember" its state after each scan call.

Most protocol scanners are implemented as a Finite State Machine (FSM). The scanner is constantly stays in one of the possible states and reacts to incoming data differently according on the current state. This project implements modbus scanner with C++ coroutines as FSM and compare coroutine-based scanner with the scanner based on traditional approach. Traditional scanner introduces several "state" subclasses each of which has virtual function on_char() and a pointer to one of the "state" instances. It looks like this:
```
class Scanner
{
    class state { virtual void on_char(uint8_t c) = 0; };
    class initial_state { virtual void on_char(uint8_t c) { /* do smth*/ } };
    ...
    class checksum_state { virtual void on_char(uint8_t c) { /* do smth*/ } };
    initial_state m_initial_state;
    ...
    checksum_state m_checksum_state;
    state* m_p_current_state {&m_initial_state};
    void scan(vector<uint8> v) { for (auto uc : v) { m_p_current_state->on_char(uc)}}
}
```
It requires a lot of boilerplate code. Every subclass has to be defined, scanner must include instances for every subclass and a pointer to current state. State transitions don't look very transparent. The code is scattered. All this may lead to errors.

On the other hand coroutine approach has only one function which is responsible for scanning process.
It significantly reduces amount of code, make it more easy to track state transitions. In fact every coroutine function operator itself defines new scanning state and it costs nothing to programmer.

So the advantage of coroutine approach in terms of programming efforts is obvious, but what about 
efficiency? Context switching on coroutine entrance and switching back on exit is not free. In this project speed test was implemented which compare time spent on scanning by coroutine-based scanner and oridinary approach scanner. It shows that coroutine-based scanner is slightly faster (43 sec corouting vs 48 seconds ordinary ).