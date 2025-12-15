class Game
{
    std::vector<std::shared_ptr<Floor>> floors;
    int currentFloor;
    string action;

    public:
        void RestartGame();
        void flush();
        void startNewGame();
        static void GameOver();
        std::shared_ptr<Floor> getCurrentFloor();
        void setCurrentFloor(std::shared_ptr<Floor>);
        string getAction();
        void MovePlayer(string cmd);
}