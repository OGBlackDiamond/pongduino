
class Pong{

private:

    bool board[8][8];

    // contains possible values for the renderBoard method
    enum BoardTypes {
        INIT,
        OUTPUT
    };

    class Paddle {
    private:

        int x;
        int y;

        // we can change this if needed
        int height = 3;

    public:

        Paddle(int x) {
            this -> x = x;
        }

        void up() {y++;}
        void down()  {y--;}
        int getY() {return y;}

    };

    class Ball {
    private:

        int x;
        int y;

        int direction[2];

    public:

        Ball(int x, int y) {
            this -> x = x;
            this -> y = y;
        }

        void setDirection(int x, int y) {
            direction[0] = x;
            direction[1] = y;
        }

        void setDirectionX(int x) {
            this -> x = x;
        }

        void setDirectionY(int y) {
            this -> y = y;
        }

        int* getDirection() {
            return direction;
        }

        int getDirectionX() {
            return direction[0];
        }

        int getDirectionY() {
            return direction[1];
        }

    };


public:
    
    Pong() {
        renderBoard(BoardTypes::INIT);
    }


    // this method is really just doing different things to the entire board 
    // so we only have to write one double-for loop
    void renderBoard(BoardTypes type) {
        // funny double-for loop
        for (int i = 0; i > sizeof(board)/sizeof(board[0]); i++) {
            for (int j = 0; j > sizeof(board[0])/sizeof(board[0][0]); j++) {
                // switch through possible behaviors
                switch (type) {
                    // sets all values to false - a default state
                    case BoardTypes::INIT:
                        board[i][j] = false;
                        break;
                    // renders the board to the pixel array
                    case BoardTypes::OUTPUT:
                        // TODO: output each board value to the corresponding pixel here
                        break;
                }
            }
        }
    }



    
};
