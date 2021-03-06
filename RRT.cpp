#include <iostream>
#include <ctime>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

enum Consts
{
    STEP_SIZE = 100,
    MAP_LEN = 1000,
    GOAL_X = 900,
    GOAL_Y = 900,
    OBSTACLE_X = 200,
    OBSTACLE_Y = 200,
    OBSTACLE_W = 600,
    OBSTACLE_H = 600,
    MAX_ITER = 1000,
};

class Dot
{
public:
    int x = 0;
    int y = 0;
    //refers to parents.
    Dot* beforePtr = NULL;
};

class DotList
{
public:
    Dot* dotPtr = NULL;
    //refers to previous element.
    DotList* beforeElement = NULL;
};

float GetDist(const float x0, const float y0, const float x1, const float y1)
{
    return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}

void main()
{
    srand((unsigned int)time(NULL));

    //set map
    Mat map(MAP_LEN, MAP_LEN, CV_8UC3, Scalar(0, 0, 0));
    line(map, Point(GOAL_X, GOAL_Y), Point(GOAL_X, MAP_LEN - 1), Scalar(0, 0, 255));
    line(map, Point(GOAL_X, GOAL_Y), Point(MAP_LEN - 1, GOAL_Y), Scalar(0, 0, 255));
    rectangle(map, Rect(OBSTACLE_X, OBSTACLE_Y, OBSTACLE_W, OBSTACLE_H), Scalar(255, 255, 255), -1);

    //initial data structure
    DotList* listTail = new DotList;
    listTail->dotPtr = new Dot;

    for (int i = 0; i < MAX_ITER; i++)
    {
        //info of minDistElement
        float minDist = std::numeric_limits<float>::max();
        Dot* minDistElement = nullptr;
        float minDistElementAng = 0;

        //temp point
        int tempX = rand() % MAP_LEN;
        int tempY = rand() % MAP_LEN;

        //find the nearest point.
        for (DotList* listIterator = listTail; listIterator != nullptr; listIterator = listIterator->beforeElement)
        {
            circle(map, Point(listIterator->dotPtr->x, listIterator->dotPtr->y), 5, Scalar(255, 255, 0), -1, LINE_AA);
            float testDist = GetDist(listIterator->dotPtr->x, listIterator->dotPtr->y, tempX, tempY);
            if (testDist > minDist)
                continue;

            minDist = testDist;
            minDistElement = listIterator->dotPtr;
            minDistElementAng = atan2(tempY - listIterator->dotPtr->y, tempX - listIterator->dotPtr->x);
        }

        //compute the coordinates of a point.
        int altX = minDistElement->x + STEP_SIZE * cos(minDistElementAng);
        int altY = minDistElement->y + STEP_SIZE * sin(minDistElementAng);
        altX = altX >= MAP_LEN ? MAP_LEN - 1 : altX;
        altY = altY >= MAP_LEN ? MAP_LEN - 1 : altY;

        //when not collided with obstacles
        if (!(altX >= OBSTACLE_X
            && altX <= OBSTACLE_X + OBSTACLE_W
            && altY >= OBSTACLE_Y
            && altY <= OBSTACLE_Y + OBSTACLE_H))
        {
            //create a new point refers to parents.
            Dot* newDot = new Dot;
            newDot->x = altX;
            newDot->y = altY;
            newDot->beforePtr = minDistElement;

            //create a new point refers to previous element.
            DotList* newTail = new DotList;
            newTail->dotPtr = newDot;
            newTail->beforeElement = listTail;
            listTail = newTail;

            //draw point and line
            circle(map, Point(altX, altY), 5, Scalar(255, 0, 255), -1, LINE_AA);
            line(map, Point(altX, altY), Point(newDot->beforePtr->x, newDot->beforePtr->y), Scalar::all(255), 1, 8, 0);

            //when the goal is reached
            if (altX > GOAL_X && altY > GOAL_Y)
            {
                //returns a path along the tree element pointing to the parent.
                for (Dot* listIterator = minDistElement; listIterator != NULL; listIterator = listIterator->beforePtr)
                {
                    circle(map, Point(listIterator->x, listIterator->y), 5, Scalar(0, 0, 255), -1, LINE_AA);
                    cout << listIterator->x << ", " << listIterator->y << endl;
                }
                break;
            }
        }
        imshow("img", map);
        waitKey(0);
    }
    imshow("img", map);
    waitKey(0);

    //free the memory using the list.
    while (listTail->beforeElement != nullptr)
    {
        DotList* beforeTail = listTail->beforeElement;
        delete listTail->dotPtr;
        delete listTail;
        listTail = beforeTail;
    }
    delete listTail->dotPtr;
    delete listTail;

    return;
}
