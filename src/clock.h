#pragma once

// returns cpu clock time in ms
// it will not be affected by NTP
// but can be used for timing based events
long long getClock();

void resetClock();

void setClock(long long value);

void addClock(long long value);
