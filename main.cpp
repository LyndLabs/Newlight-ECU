#include <map>

// DEFINE FUNCTION LOGIC LATERS
bool checkSystems() { return true; }
void reboot() {}
void shutdown() {}
void open(int gpio) {}
bool checkPressureDecay(int seconds) { return true; }
void checkPressure(int gpio) {}
void shutoffValves() {}
void pressureDecrease(int gpio) {}
void closeAll() {}

// DEFINE REAL PINS LATER // inert gas, solenoids, pressure transmitter
constexpr int IGS01 = 0;
constexpr int IGS03 = 1;
constexpr int IGS04 = 2;

constexpr int SV102 = 3;
constexpr int SV106 = 4;
constexpr int SV101 = 5;
constexpr int SV109 = 6;

constexpr int PT102 = 7;
constexpr int PT106_111 = 7;

constexpr int PY102 = 9;

enum class StateType {
    CheckSystems,
    Open,
    CheckPressureDecay,
    CheckPressure,
    ShutoffValves,
    PressureDecrease,
    CloseAll,
    Reboot,
    Shutdown,
    End
};

struct StateNode {
    StateType type;
    int param = -1;
    StateType onTrue = StateType::End;
    StateType onFalse = StateType::End;
};


// PURGE
using StateGraph = std::map<StateType, StateNode>;
StateGraph buildPurgeGraph() {
    return {
        {StateType::CheckSystems,       {StateType::CheckSystems,     -1,           StateType::Open,        StateType::Reboot}},
        {StateType::Open,               {StateType::Open,             IGS01,        StateType::Open,        StateType::End}},
        {StateType::Open,               {StateType::Open,             SV102,        StateType::CheckPressureDecay, StateType::End}},
        {StateType::CheckPressureDecay, {StateType::CheckPressureDecay, 0,          StateType::Open,        StateType::Shutdown}},
        {StateType::Open,               {StateType::Open,             SV106,        StateType::Open,        StateType::End}},
        {StateType::Open,               {StateType::Open,             IGS03,        StateType::Open,        StateType::End}},
        {StateType::Open,               {StateType::Open,             IGS04,        StateType::CheckPressure, StateType::End}},
        {StateType::CheckPressure,      {StateType::CheckPressure,    PT102,        StateType::CheckPressure, StateType::End}},
        {StateType::CheckPressure,      {StateType::CheckPressure,    PT106_111,    StateType::ShutoffValves, StateType::End}},
        {StateType::ShutoffValves,      {StateType::ShutoffValves,    -1,           StateType::CheckPressureDecay, StateType::End}},
        {StateType::CheckPressureDecay, {StateType::CheckPressureDecay, 60,         StateType::Open,        StateType::Shutdown}},
        {StateType::Open,               {StateType::Open,             SV101,        StateType::Open,        StateType::End}},
        {StateType::Open,               {StateType::Open,             SV109,        StateType::PressureDecrease, StateType::End}},
        {StateType::PressureDecrease,   {StateType::PressureDecrease, PY102,        StateType::PressureDecrease, StateType::End}},
        {StateType::PressureDecrease,   {StateType::PressureDecrease, PT106_111,    StateType::CloseAll,    StateType::End}},
        {StateType::CloseAll,           {StateType::CloseAll,         -1,           StateType::End,         StateType::End}},
        {StateType::Reboot,             {StateType::Reboot,           -1,           StateType::End,         StateType::End}},
        {StateType::Shutdown,           {StateType::Shutdown,         -1,           StateType::End,         StateType::End}},
    };
}

// START 
// NORMAL SHUTOFF
// EMERG SHUTOFF
// MONITOR

void runStateMachine(const StateGraph& graph) {
    // iterate from beginning to end of statetypes
    StateType current = StateType::CheckSystems;
    while (current != StateType::End) {
        const auto& node = graph.at(current);
        bool result = true;
        switch (node.type) {
            case StateType::CheckSystems:       result = checkSystems(); break;
            case StateType::Open:               open(node.param); break;
            case StateType::CheckPressureDecay: result = checkPressureDecay(node.param); break;
            case StateType::CheckPressure:      checkPressure(node.param); break;
            case StateType::ShutoffValves:      shutoffValves(); break;
            case StateType::PressureDecrease:   pressureDecrease(node.param); break;
            case StateType::CloseAll:           closeAll(); break;
            case StateType::Reboot:             reboot(); break;
            case StateType::Shutdown:           shutdown(); break;
            default:                            break;
        }
        if (node.type == StateType::CheckSystems || node.type == StateType::CheckPressureDecay)
            current = result ? node.onTrue : node.onFalse;
        else
            current = node.onTrue;
    }
}

int main() {
    auto graph = buildPurgeGraph();
    runStateMachine(graph);
    return 0;
}
