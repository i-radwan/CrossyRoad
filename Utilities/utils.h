#ifndef UTILS
#define UTILS

#include <vector>
#include "../Models/Car.h"

using namespace std;
static enum LaneType {
    SAFE_LANE, NORMAL_LANE
};
class Constants {
public:
    constexpr static const GLfloat TREE_Y = 0.432f;
    constexpr static const GLfloat CAR_Y = 1.065;
    constexpr static const GLfloat TRUCK_Y = 0.45;
    constexpr static const GLfloat COIN_Y = 1.2;
    constexpr static const GLuint SHADOW_WIDTH = 4400;
    constexpr static const GLuint SHADOW_HEIGHT = 4000;
    constexpr static const GLint gameWidth = 750;
    constexpr static const GLint gameHeight = 800;
};
struct Lane {
    float laneZPos;
    LaneType type;
    bool drawnBefore = 0;
    bool isVisited = false;
    bool isTruck = false;
    bool hasCoin = false;
    float coinXPosition = 0;
    float coinRotation = 0;
    bool isCoinConsumed = false;
    float treeXpos = -3;
    bool treeDrawn = 0;
    Lane(){
        setTreeXPos();
        
    }
    void setTreeXPos(){
        // 77 is the first number popped in my mind, ther is no scientific reason behind.
        treeXpos = ( (rand() * rand() * rand() * rand())%77 * (rand() > 6 ? -1 : 1 ))%15;
    }
    void setCoinPosition(){
        if(laneZPos < -4 && !hasCoin && !drawnBefore){
            GLint randNum = (10 + (rand() % (int)(2000 - 10 + 1)));
            GLboolean addCoin = (randNum % 3 == 0);
            if(addCoin){
                GLfloat coinX = -6 + (rand() % (int)(9 - -6 + 1));
                if(abs(coinX - treeXpos) > 10){
                    coinXPosition = coinX;
                    hasCoin = true;
                }
            }
        }
    }
    void moveCar (){
        laneCarXPosition += laneCarSpeed;
        if(laneCarXPosition < -25){
            laneCarXPosition =  14 + (rand() % (int)(30 - 14 + 1));
        }
    }
    void rotateCoin(){
        coinRotation++;
        if(coinRotation > 360){
            coinRotation -= 360;
        }
    }
    void setLaneCarSpeed(GLfloat speed){
        laneCarSpeed = speed;
        if(laneCarSpeed < - 0.7){
            laneCarSpeed = -0.7;
        }
    }
    void setLaneCarXPosition (GLfloat xPos){
        this->laneCarXPosition = xPos;
    }
    GLfloat getLaneCarXPosition(){return this->laneCarXPosition;}
    GLfloat getLaneCarSpeed(){return this->laneCarSpeed;}
private:
    GLfloat laneCarXPosition =  -2 + (rand() % (int)(22 - 2 + 1));
    GLfloat laneCarSpeed = -0.01;
};


void generateLaneData(int laneData[]){
    
    int count[2] = {0,1};
    int randomNumber;
    int maxAllowed[2] = {2,5};
    laneData[0] = 1;
    count[1]++;
    int previousLane = 1;
    int i = 1;
    for(; i < 48; i++){
        randomNumber = rand();
        int randomLane = randomNumber%2;
        if(randomLane == previousLane && count[randomLane] < maxAllowed[randomLane]){
            count[randomLane]++;
            laneData[i] = randomLane;
            count[1-randomLane] = 0;
        }
        else if( randomLane == previousLane && count[randomLane] > maxAllowed[randomLane]){
            laneData[i] = 1 - randomLane;//Swap
            count[randomLane] = 0;
            count[1-randomLane] = 1;
            previousLane = 1- randomLane;
        }
        else if( randomLane == previousLane && count[randomLane] == maxAllowed[randomLane]){
            count[randomLane] = 0;
            laneData[i] =1 -randomLane;
            previousLane = randomLane;
            count[1-randomLane] = 1;
        }
        else{
            count[randomLane] = 1;
            laneData[i] = randomLane;
            count[1-randomLane] = 0;
            previousLane = randomLane;
        }
    }
}

class Utilities{
public:
    static void generateLanesAlgorithm(vector<Lane>& lanesArray) {
        int lanesData[] = {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe Lane, 1 normal Lane
        float laneZ = -1.3;
        for(int i = 0 ;i < 52; i++){
            int random= (1 + rand() % (200 - 1));
            
            Lane s;
            s.type = static_cast<LaneType>(lanesData[i]);
            s.drawnBefore = 0;
            s.laneZPos = laneZ;
            s.setCoinPosition();
            if(random % 3 == 1 || random % 5 == 1 ){
                s.isTruck = true;
            }
            // Set speed ToDo fix this
            if(i <= 20 == 0){
                s.setLaneCarSpeed(s.getLaneCarSpeed() - (i + 1) * 0.001);
            }else if(i > 20 && i < 40) {
                s.setLaneCarSpeed(s.getLaneCarSpeed() - (i + 1) * 0.0013);
            }else{
                s.setLaneCarSpeed(s.getLaneCarSpeed() - (i + 1) * 0.001);
            }
            if(i > 15)
                s.setLaneCarXPosition(-25 + (rand() % (int)(25 + 25 + 1)));
            
            lanesArray.push_back(s);
            // Z operations
            if((i < 51 && lanesData[i] == LaneType::NORMAL_LANE && lanesData[i+1] == LaneType::SAFE_LANE) || (i < 51 && lanesData[i] == LaneType::SAFE_LANE && lanesData[i+1] ==LaneType::NORMAL_LANE)){
                laneZ -=1.5;
            }
            else{
                laneZ -= 1;
            }
            if(i < 51 && lanesData[i] == LaneType::NORMAL_LANE && lanesData[i+1] == LaneType::NORMAL_LANE){
                laneZ -= 1.1;
            }
            
        }
    }
    static void addMoreLanes(vector<Lane>& lanesArray) {
        int lanesData[50];
        //autogeneration
        generateLaneData(lanesData);
        vector<Lane> newLanesArray;
        GLfloat lastCarSpeed = 0;
        float lastLaneZPos = 0;
        int lastLaneType = 0;
        for(int j = 32, i = 0 ; j < 52; j++, i++){
            Lane s =lanesArray[j];
            newLanesArray.push_back(s);
            lastCarSpeed = min(lanesArray[j].getLaneCarSpeed(), lastCarSpeed);
            lastLaneZPos = s.laneZPos;
            lastLaneType = s.type;
        }
        for(int i = 0, j = (52-32); i < 32; i++, j++){
            if(i == 0){
                // Z operations
                if((lastLaneType == LaneType::NORMAL_LANE && lanesData[i] == LaneType::SAFE_LANE) || (lastLaneType == LaneType::SAFE_LANE && lanesData[i] == LaneType::NORMAL_LANE)){
                    lastLaneZPos -=1.5;
                }
                else{
                    lastLaneZPos -= 1;
                }
                if(lastLaneType == LaneType::NORMAL_LANE && lanesData[i] == LaneType::NORMAL_LANE){
                    lastLaneZPos -= 1.1;
                }
            }
            int random = (1 + rand() % (200 - 1));
            
            Lane s;
            s.type = static_cast<LaneType>(lanesData[i]);
            s.drawnBefore = 0;
            s.laneZPos = lastLaneZPos;
            s.setCoinPosition();
            if(random % 3 == 1 || random % 5 == 1 ){
                s.isTruck = true;
            }
            if(i <= 20){
                s.setLaneCarSpeed(s.getLaneCarSpeed() + (lastCarSpeed - (i + 1) * 0.001));
            }else if(i > 20 && i < 40) {
                s.setLaneCarSpeed(s.getLaneCarSpeed() + (lastCarSpeed - (i + 1) * 0.0013));
            }
            s.setLaneCarXPosition(-25 + (rand() % (int)(25 + 25 + 1)));
            newLanesArray.push_back(s);
            // Z operations
            if((i < 31 && lanesData[i] == LaneType::NORMAL_LANE && lanesData[i+1] == LaneType::SAFE_LANE) || (i < 31 && lanesData[i] == LaneType::SAFE_LANE && lanesData[i+1] == LaneType::NORMAL_LANE)){
                lastLaneZPos -=1.5;
            }
            else{
                lastLaneZPos -= 1;
            }
            if(i < 31 && lanesData[i] == LaneType::NORMAL_LANE && lanesData[i+1] == LaneType::NORMAL_LANE){
                lastLaneZPos -= 1.1;
            }
        }
        lanesArray.clear();
        lanesArray = newLanesArray;
    }
    
};
#endif
