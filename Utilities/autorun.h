struct bfsNode{
    bfsNode* next;
    bfsNode* parent;
    int nodeX = 0, laneZIndex = 0;
    int move = 0;
    int accumulatedFramesCount =0;
    bfsNode(){
        next = parent = 0;
    }
};
class AutoRun{
    public:
    bool isAutoRunEnabled;
    vector<Lane>& lanesArray;
    int penmove;
    float targetPenX ;
    Penguin* testPen;
    bfsNode* currentbfsNode;    

    
    AutoRun(vector<Lane>& lanesVector, bool enabled = false):lanesArray(lanesVector), isAutoRunEnabled(enabled){
        currentbfsNode = new bfsNode();
    }
    
    bfsNode* autoRunAlgo() {
        // Get current penguin lane index
        // Get next safe index
        // BFS from current index to next index
        int nextSafeLaneIdx = Utilities::nextSafeLaneIdx(lanesArray, testPen->getCurrentLane());
        
        vector<vector<int> > visited(50, std::vector<int> ( 17, 0)); // 50 * 17 vector to hold visited
        int requZ = nextSafeLaneIdx;
        currentbfsNode->laneZIndex = testPen->getCurrentLane();
        
        visited[currentbfsNode->laneZIndex][currentbfsNode->nodeX] = 1;

        queue<bfsNode*> q;
        q.push(currentbfsNode);
        bfsNode* tmp;
        
        float backupX = testPen->getPenX();
        float backupZ = testPen->getPenZ();
        int backupLaneIndex = testPen->getCurrentLane();
        
        currentbfsNode->accumulatedFramesCount = 0;
        while(!q.empty()){
            tmp = q.front();
            q.pop();
            if (tmp->laneZIndex == requZ) { // check if target is reached
                testPen->setPenZ(backupZ);
                testPen->setPenX(backupX);
                testPen->setCurrentLaneIndex(backupLaneIndex);
                return tmp;
            }
            Lane nextLane, previousLane, currentLane;
            nextLane = lanesArray[tmp->laneZIndex +1];
            currentLane = lanesArray[tmp->laneZIndex];
            previousLane = lanesArray[tmp->laneZIndex -1];
            
            // Simulate moving
            testPen->setPenZ(nextLane.laneZPos);
            testPen->setPenX(tmp->nodeX-9);
            testPen->setCurrentLaneIndex(tmp->laneZIndex +1);
            float framesToBeAdded = 0;
//
//            float nextCarX = nextLane.getLaneCarXPosition();
//            int framesToAdd = tmp->accumulatedFramesCount;
//            while (framesToAdd > 0) {
//                nextCarX += nextLane.getLaneCarSpeed();
//                if(nextCarX < -25){
//                    nextCarX = nextLane.originalCarXPosition;
//                }
//                framesToAdd--;
//            }
//            if(nextCarX > tmp->nodeX -9+2){
//                if(currentLane.type == SAFE_LANE && nextLane.type == SAFE_LANE)
//                    framesToBeAdded = 25.0 * 1.5;
//                else if (currentLane.type == SAFE_LANE && nextLane.type == NORMAL_LANE ||
//                         currentLane.type == NORMAL_LANE && nextLane.type == SAFE_LANE)
//                    framesToBeAdded = 38.0 * 1.5;
//                else if (currentLane.type == NORMAL_LANE && nextLane.type == NORMAL_LANE)
//                    framesToBeAdded = 53.0 * 1.5;
//                
//            }else { // before
//                if(currentLane.type == SAFE_LANE && nextLane.type == SAFE_LANE)
//                    framesToBeAdded = 25.0/2;
//                else if (currentLane.type == SAFE_LANE && nextLane.type == NORMAL_LANE ||
//                         currentLane.type == NORMAL_LANE && nextLane.type == SAFE_LANE)
//                    framesToBeAdded = 38.0/2;
//                else if (currentLane.type == NORMAL_LANE && nextLane.type == NORMAL_LANE)
//                    framesToBeAdded = 53.0/2;
//                
//            }
            float horzFramesCount = 0;
//            float currentLaneCarX = currentLane.getLaneCarXPosition() + currentLane.getLaneCarSpeed() * tmp->accumulatedFramesCount;
//            
//            if(currentLaneCarX > tmp->nodeX -9){
//                horzFramesCount = 6;
//            } else { // before
//                horzFramesCount = 6;
//            }
//
            if(tmp->laneZIndex+1 < 50 && !visited[tmp->laneZIndex+1][tmp->nodeX] &&
               testPen->detectCollisionWithAutoRun(lanesArray, tmp->accumulatedFramesCount+framesToBeAdded) == NO_COLLISION
               ){
                
                if(!(lanesArray[tmp->laneZIndex +1].type == SAFE_LANE &&  abs(lanesArray[tmp->laneZIndex +1].treeXpos - (tmp->nodeX-9)) < 1.3)){
                    
                    bfsNode* newNode = new bfsNode;
                    newNode->nodeX = tmp->nodeX;
                    newNode->laneZIndex = tmp->laneZIndex+1;
                    newNode->move = 1;
                    newNode->parent = tmp;
                    newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+framesToBeAdded;
                    tmp->next = newNode;
                    q.push(newNode);
                    visited[tmp->laneZIndex+1][tmp->nodeX] = 1;
                }}
            testPen->setCurrentLaneIndex(tmp->laneZIndex);
            testPen->setPenZ(currentLane.laneZPos);
            testPen->setPenX(tmp->nodeX-7);
            
            if(tmp->nodeX+1 < 17 &&!visited[tmp->laneZIndex][tmp->nodeX+1] &&
               testPen->detectCollisionWithAutoRun(lanesArray,tmp->accumulatedFramesCount+horzFramesCount) == NO_COLLISION){
                if((lanesArray[tmp->laneZIndex].type == LaneType::SAFE_LANE
                    && (abs((tmp->nodeX-9)+1  - lanesArray[tmp->laneZIndex].treeXpos) > 1.3) || ((tmp->nodeX-9)+1 > lanesArray[tmp->laneZIndex].treeXpos)) || lanesArray[tmp->laneZIndex].type == LaneType::NORMAL_LANE){
                    
                    bfsNode* newNode = new bfsNode;
                    newNode->nodeX = tmp->nodeX+1;
                    newNode->laneZIndex = tmp->laneZIndex;
                    newNode->parent = tmp;
                    newNode->move = 3;
                    newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+horzFramesCount;
                    tmp->next = newNode;
                    q.push(newNode);
                    visited[tmp->laneZIndex][tmp->nodeX+1] = 1;
                }
            }
            testPen->setCurrentLaneIndex(tmp->laneZIndex);
            testPen->setPenZ(currentLane.laneZPos);
            testPen->setPenX(tmp->nodeX-9);
            if(tmp->nodeX-1 > 1 &&!visited[tmp->laneZIndex][tmp->nodeX-1]&&
               testPen->detectCollisionWithAutoRun(lanesArray, tmp->accumulatedFramesCount+horzFramesCount) == NO_COLLISION){
                
                if((lanesArray[tmp->laneZIndex].type == LaneType::SAFE_LANE
                    && (abs((tmp->nodeX-9)-1  - lanesArray[tmp->laneZIndex].treeXpos) > 1.3) || ((tmp->nodeX-9)-1 < lanesArray[tmp->laneZIndex].treeXpos)) || lanesArray[tmp->laneZIndex].type == LaneType::NORMAL_LANE){
                    bfsNode* newNode = new bfsNode;
                    newNode->nodeX = tmp->nodeX-1;
                    newNode->laneZIndex = tmp->laneZIndex;
                    newNode->parent = tmp;
                    newNode->move = 4;
                    tmp->next = newNode;
                    newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+horzFramesCount;
                    q.push(newNode);
                    visited[tmp->laneZIndex][tmp->nodeX-1] = 1;
                }
            }
            
            testPen->setPenZ(previousLane.laneZPos);
            testPen->setCurrentLaneIndex(tmp->laneZIndex - 1);
            testPen->setPenX((tmp->nodeX-9));
            
            if(tmp->laneZIndex-1 > 0 && !visited[tmp->laneZIndex-1][tmp->nodeX] &&
               testPen->detectCollisionWithAutoRun(lanesArray, tmp->accumulatedFramesCount+framesToBeAdded) == NO_COLLISION){
                
                if(!(lanesArray[tmp->laneZIndex -1].type == SAFE_LANE &&  abs(lanesArray[tmp->laneZIndex -1].treeXpos - (tmp->nodeX-9)) < 1.3)){
                    
                    bfsNode* newNode = new bfsNode;
                    newNode->nodeX = tmp->nodeX;
                    newNode->laneZIndex = tmp->laneZIndex-1;
                    newNode->parent = tmp;
                    newNode->move = 2;
                    newNode->accumulatedFramesCount = tmp->accumulatedFramesCount+framesToBeAdded;
                    tmp->next = newNode;
                    q.push(newNode);
                    
                    visited[tmp->laneZIndex-1][tmp->nodeX] = 1;
                }
            }
        }
        cout << "NOWAY!"<<endl;
        testPen->setPenZ(backupZ);
        testPen->setPenX(backupX);
        testPen->setCurrentLaneIndex(backupLaneIndex);
        return 0;
    }
};
