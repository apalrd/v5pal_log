% Compute the new state vector from the old state and dt
lastState = curState;
lastCov = curCov;

% Compute velocity from velocity and accel
curState(3) = curState(3) + curState(6) * dt;
curState(4) = curState(4) + curState(7) * dt;
curState(5) = curState(5) + curState(8) * dt;

% Compute CoV velocity from CoV velocity and accel
curCov(3) = curCov(3) + curCov(6) * dt;
curCov(4) = curCov(4) + curCov(7) * dt;
curCov(5) = curCov(5) + curCov(8) * dt;

% Compute new heading from integration
curState(2) = curState(2) + curState(5) * dt;
curCov(2) = curCov(2) + curCov(5) * dt;

% Coordinate transform from forward + normal into x/y
df = curState(3) * dt;
dn = curState(4) * dt;
dsin = sin(curState(5));
dcos = cos(curState(5));