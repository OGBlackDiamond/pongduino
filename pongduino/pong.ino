
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

        int score;

        // we can change this if needed
        int height = 3;

    public:

        Paddle() {
            x = -1;
            y = -1;
        }

        Paddle(int x) {
            this -> x = x;
            y = 3;
            score = 0;
        }

        void up() {y++;}
        void down()  {y--;}
        void scorePoint() {score++;}
        int getY() {return y;}
        int getHeight() {return height;}

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

        void move() {
            x += direction[0];
            y += direction[1];
        }

        void checkNextColision(Paddle *p1, Paddle *p2) {
            // if the ball is not near an edge, don't bother calculating anything
            if (x <= 6 && x >= 2) return;

            // decide which paddle to detect based on ball's position
            Paddle player = x > 6 ? *p2 : *p1;

            // the x and y positions the ball will be in on the next tick
            int xStep = x + direction[0];
            int yStep = y + direction[1];

            if (yStep >= player.getY() && yStep <= player.getY()+player.getHeight()) {
                direction[0] *= -1; 
                direction[1] = x - (player.getY() + (player.getHeight() / 2));
                // cap the vertical speed
                if (direction[1] > 3) {direction[1] = 3;}
            }

            if (yStep < 0 || yStep > 7) {
                direction[0] *= -1;
                // account for any overflow that would happen on the next tick
                y -= direction[0];
            }

            if (xStep < 0 || xStep > 7) {
                direction[0] *= -1;
                direction[1] = 1;
                x = 3;
                y = 3;
                player.scorePoint();
            }


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

    Paddle player1;
    Paddle player2;
    Ball ball;

public:
    
    Pong(): ball(3, 3), player1(0), player2(7) {
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
    // funny pyramid function ^

    void tick() {
        ball.move(); 
        // TODO: accept user input and use the up() and down() methods on each paddle to control them 
        ball.checkNextColision(&player1, &player2);
        renderBoard(BoardTypes::OUTPUT);
    }
    
};
