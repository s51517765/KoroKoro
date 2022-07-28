//ファイルの分割
// https://bellitblog.net/2021/01/07/arduino-file-create/

void initRand(void);
void printMaze();
void dig(int i, int j);
void createMaze();
int returnMaze(int j, int i);
const int MEIRO_WIDTH = 13;
const int MEIRO_HEIGHT = 11;