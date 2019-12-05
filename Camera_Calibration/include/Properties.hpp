//Defining the board
#define onlyIntrinsics false
#define numBoards 5

#define numCornersHor 8
#define numCornersVer 6

#define pointsFileLocation ((onlyIntrinsics)?("../Matlab scripts/pointsForIntrinsic.txt"):("../Matlab scripts/points.txt"))

//how many times the CalibrateCamera function is called
#define numRuns 1

//All the displayed images will be resized with that factor
#define resizeFactor 0.3f

//If it is true, we have to accept each found chessboard
#define requireUserInput true
