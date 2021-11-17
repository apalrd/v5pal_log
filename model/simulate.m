% Simulation function
clear
load('data000047.mat');
% Initialize states and cov based on starting assumptions
curState = [-1.47, 1.207, 275/180*3.14159, 0, 0, 0, 0, 0, 0];
curCov = [0.05, 0.05, 5/180*3.14159, 0.01, 0.01, 0.03, 0.05, 0.05, 0.1];

% Step through the array, do prediction and update
dt = 0.02;
model()
