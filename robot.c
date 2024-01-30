#include "robot.h"

void setup_robot(struct Robot *robot) // change from void setup_robot(struct Robot *robot, int startX, int startY){
{
    robot->x = 330;
    robot->y = 360;
    robot->true_x = 350;
    robot->true_y = 360;
    robot->width = ROBOT_WIDTH;
    robot->height = ROBOT_HEIGHT;
    robot->direction = 90;
    robot->angle = 90;
    robot->currentSpeed = 0;
    robot->count = 0;

    printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}

int robot_off_screen(struct Robot *robot)
{
    if (robot->x < 0 || robot->y < 0)
    {
        return 0;
    }
    if (robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT)
    {
        return 0;
    }
    return 1;
}

int checkRobotHitWall(struct Robot *robot, struct Wall *wall)
{

    int overlap = checkOverlap(robot->x, robot->width, robot->y, robot->height,
                               wall->x, wall->width, wall->y, wall->height);

    return overlap;
}

int checkRobotHitWalls(struct Robot *robot, struct Wall_collection *head)
{
    struct Wall_collection *ptr = head;
    int hit = 0;

    while (ptr != NULL)
    {
        hit = (hit || checkRobotHitWall(robot, &ptr->wall));
        ptr = ptr->next;
    }
    return hit;
}

int checkRobotReachedEnd(struct Robot *robot, int x, int y, int width, int height)
{
    int overlap = checkOverlap(robot->x, robot->width, robot->y, robot->height,
                               x, width, y, height);
    return overlap;
}

void robotCrash(struct Robot *robot)
{
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot *robot, int msec)
{
    robot->currentSpeed = 0;
    if (!robot->crashed)
    {
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec / 1000, msec % 1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall *wall)
{
    // viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x, 2, y, sensorSensitivityLength,
                               wall->x, wall->width, wall->y, wall->height);

    return overlap;
}

int checkRobotSensorFrontCentreAllWalls(struct Robot *robot, struct Wall_collection *head)
{
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;

    int sensorSensitivityLength = floor(SENSOR_VISION / 5);

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        // xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        // yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL)
        {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorLeftAllWalls(struct Robot *robot, struct Wall_collection *head)
{
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;
    sensorSensitivityLength = floor(SENSOR_VISION / 5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        // xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        // yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX + (+ROBOT_WIDTH / 2) * cos((robot->angle - 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle - 90) * PI / 180));
        yDir = round(robotCentreY + (+ROBOT_WIDTH / 2) * sin((robot->angle - 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle - 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL)
        {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorRightAllWalls(struct Robot *robot, struct Wall_collection *head)
{
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;
    score = 0;
    sensorSensitivityLength = floor(SENSOR_VISION / 5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        // xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        // yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle + 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * sin((robot->angle + 90) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle + 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensorSensitivityLength * i) * cos((robot->angle + 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;
        hit = 0;

        while (ptr != NULL)
        {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

void robotUpdate(struct SDL_Renderer *renderer, struct Robot *robot)
{
    double xDir, yDir;

    int robotCentreX, robotCentreY, xTR, yTR, xTL, yTL, xBR, yBR, xBL, yBL;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    /*
    //Other Display options:
    // The actual square which the robot is tested against (not so nice visually with turns, but easier
    // to test overlap
    SDL_Rect rect = {robot->x, robot->y, robot->height, robot->width};
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
    */
    /*
    //Center Line of Robot. Line shows the direction robot is facing
    xDir = -30 * sin(-robot->angle*PI/180);
    yDir = -30 * cos(-robot->angle*PI/180);
    xDirInt = robot->x+ROBOT_WIDTH/2+ (int) xDir;
    yDirInt = robot->y+ROBOT_HEIGHT/2+ (int) yDir;
    SDL_RenderDrawLine(renderer,robot->x+ROBOT_WIDTH/2, robot->y+ROBOT_HEIGHT/2, xDirInt, yDirInt);
    */

    // Rotating Square
    // Vector rotation to work out corners x2 = x1cos(angle)-y1sin(angle), y2 = x1sin(angle)+y1cos(angle)
    robotCentreX = robot->x + ROBOT_WIDTH / 2;
    robotCentreY = robot->y + ROBOT_HEIGHT / 2;

    xDir = round(robotCentreX + (ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (-ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (-ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xTR = (int)xDir;
    yTR = (int)yDir;

    xDir = round(robotCentreX + (ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xBR = (int)xDir;
    yBR = (int)yDir;

    xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xBL = (int)xDir;
    yBL = (int)yDir;

    xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle) * PI / 180) - (-ROBOT_HEIGHT / 2) * sin((robot->angle) * PI / 180));
    yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle) * PI / 180) + (-ROBOT_HEIGHT / 2) * cos((robot->angle) * PI / 180));
    xTL = (int)xDir;
    yTL = (int)yDir;

    SDL_RenderDrawLine(renderer, xTR, yTR, xBR, yBR);
    SDL_RenderDrawLine(renderer, xBR, yBR, xBL, yBL);
    SDL_RenderDrawLine(renderer, xBL, yBL, xTL, yTL);
    SDL_RenderDrawLine(renderer, xTL, yTL, xTR, yTR);

    // Front Centre Sensor
    int sensor_sensitivity = floor(SENSOR_VISION / 5);
    int i;
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = {xTL, yTL, 2, sensor_sensitivity};
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Left Sensor
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX + (+ROBOT_WIDTH / 2) * cos((robot->angle - 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle - 90) * PI / 180));
        yDir = round(robotCentreY + (+ROBOT_WIDTH / 2) * sin((robot->angle - 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle - 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = {xTL, yTL, 2, sensor_sensitivity};
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Right Sensor
    for (i = 0; i < 5; i++)
    {
        xDir = round(robotCentreX + (-ROBOT_WIDTH / 2) * cos((robot->angle + 90) * PI / 180) - (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * sin((robot->angle + 90) * PI / 180));
        yDir = round(robotCentreY + (-ROBOT_WIDTH / 2) * sin((robot->angle + 90) * PI / 180) + (-ROBOT_HEIGHT / 2 - SENSOR_VISION + sensor_sensitivity * i) * cos((robot->angle + 90) * PI / 180));
        xTL = (int)xDir;
        yTL = (int)yDir;

        SDL_Rect rect = {xTL, yTL, 2, sensor_sensitivity};
        SDL_SetRenderDrawColor(renderer, 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 80 + (20 * (5 - i)), 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
    }

    // xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle)*PI/180));
    // yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle)*PI/180));
}

void robotMotorMove(struct Robot *robot, int crashed)
{
    double x_offset, y_offset;
    if (crashed)
        robot->currentSpeed = 0;
    else
    {
        switch (robot->direction)
        {
        case UP:
            robot->currentSpeed += DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed > MAX_ROBOT_SPEED)
                robot->currentSpeed = MAX_ROBOT_SPEED;
            break;
        case DOWN:
            robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                robot->currentSpeed = -MAX_ROBOT_SPEED;
            break;
        case LEFT:
            robot->angle = (robot->angle + 360 - DEFAULT_ANGLE_CHANGE) % 360;
            break;
        case RIGHT:
            robot->angle = (robot->angle + DEFAULT_ANGLE_CHANGE) % 360;
            break;
        }
    }
    robot->direction = 0;
    x_offset = (-robot->currentSpeed * sin(-robot->angle * PI / 180));
    y_offset = (-robot->currentSpeed * cos(-robot->angle * PI / 180));

    robot->true_x += x_offset;
    robot->true_y += y_offset;

    x_offset = round(robot->true_x);
    y_offset = round(robot->true_y);

    robot->x = (int)x_offset;
    robot->y = (int)y_offset;
}

void frontWallFound(struct Robot *robot, int front_centre_sensor, int left_sensor, int right_sensor)
{
    switch (robot->count)
    {
    case 0:
        printf("SLOWING DOWN, LEFT: %d, RIGHT: %d\n", left_sensor, right_sensor);
        // If when slowing down and robot gets too close to either left or right, move opposite way
        if ((left_sensor >= 3) && (right_sensor < 3))
        {
            robot->direction = RIGHT;
            robot->found_front_wall = 0;
            break;
        }
        else if ((right_sensor >= 3) && (left_sensor < 3))
        {
            robot->direction = LEFT;
            robot->found_front_wall = 0;
            break;
        }

        if (robot->currentSpeed >= -1 && robot->timer == 0)
        { // slow down until a speed of -2
            robot->direction = DOWN;
        }
        else if (robot->currentSpeed < 0 && robot->timer < 2)
        { // back off for 3 ticks
            (robot->timer)++;
        }
        else if (robot->currentSpeed < 0 && robot->timer < 5)
        { // go to a stop
            (robot->timer)++;
            robot->direction = UP;
        }
        else
        {
            robot->count++;
            robot->timer = 0;
            robot->turn_count = 0;
        }
        break;
    case 1: // Turn to the left
        if ((right_sensor >= 1) || (robot->turn_count == 6))
        {
            (robot->left_score) = robot->turn_count;
            (robot->count)++;
            printf("Left turn score = %d\n", robot->turn_count);
        }
        else
        {
            robot->direction = LEFT;
            (robot->turn_count)++;
        }
        break;
    case 2: // undo left turn
        if (robot->turn_count == 0)
        {
            (robot->count)++;
        }
        else
        {
            robot->direction = RIGHT;
            (robot->turn_count)--;
        }
        break;

    case 3: // Turn to the right
        if ((left_sensor >= 1) || (robot->turn_count == 6))
        {
            (robot->right_score) = robot->turn_count;
            (robot->count)++;
            printf("Right turn score = %d\n", robot->turn_count);
        }
        else
        {
            robot->direction = RIGHT;
            (robot->turn_count)++;
        }
        break;
    case 4: // undo right turn
        if (robot->turn_count == 0)
        {
            (robot->count)++;
        }
        else
        {
            robot->direction = LEFT;
            (robot->turn_count)--;
        }
        break;

    case 5: // back off
        robot->direction = DOWN;
        (robot->count)++;
        break;

    case 6: // stop
        robot->direction = UP;
        (robot->count)++;
        break;
    case 7:
        printf("LEFT SCORE %d, RIGHT SCORE %d\n", robot->left_score, robot->right_score);
        if ((robot->turn_count) == 6)
        { // Committed to a direction
            robot->timer = 0;
            robot->turn_count = 0;
            (robot->count)++;
            // robot->found_front_wall = 0;
        }
        else if ((robot->right_score) > (robot->left_score))
        {
            robot->direction = LEFT;
            (robot->turn_count)++;
        }
        else if ((robot->left_score) > (robot->right_score))
        {
            robot->direction = RIGHT;
            (robot->turn_count)++;
        }
        else if (robot->left_score == robot->right_score)
        {
            printf("LEFT AND RIGHT SCORE EQUAL, RANDOMLY ");
            int r = rand();
            r = r % 2;
            if (r == 1)
            {
                printf("CHOOSING LEFT\n");
                (robot->right_score)++;
                robot->direction = LEFT;
            }
            else
            {
                printf("CHOOSING RIGHT\n");
                (robot->left_score)++;
                robot->direction = RIGHT;
            }
            (robot->turn_count)++;
        }
        break;

    case 8: // accelerate to 5
        if (front_centre_sensor >= 1)
        {
            robot->direction = DOWN;
            (robot->count)++;
        }
        else if (robot->currentSpeed < 6)
        {
            robot->direction = UP;
        }
        else
        {
            (robot->count)++;
        }
    case 9:
        printf("CHECKING FOR FRONT WALL, TIMER %d\n", robot->timer);
        if (front_centre_sensor >= 1)
        {
            robot->direction = DOWN;
            (robot->count)++;
            break;
        }
        else if (robot->timer >= 10)
        {
            robot->found_front_wall = 0;
            break;
        }
        else if (robot->currentSpeed > 0)
        {
            (robot->timer)++;
        }
        break;
    case 10: // slow down to halt
        if (robot->currentSpeed > 0)
        {
            robot->direction = DOWN;
        }
        else
        {
            (robot->count)++;
        }
        break;
    case 11: // Turn 180
        if (robot->turn_count >= 12)
        {
            robot->found_front_wall = 0;
        }
        else
        {
            robot->direction = LEFT; // LEFT OR RIGHT DOESN'T MATTER
            (robot->turn_count)++;
        }
        break;
    }
}

void robotAutoMotorMove(struct Robot *robot, int front_centre_sensor, int left_sensor, int right_sensor)
{
    if (robot->found_front_wall == 1)
    {
        frontWallFound(robot, front_centre_sensor, left_sensor, right_sensor);
    }
    else if ((left_sensor >= 3) && (right_sensor < 3))
    {
        robot->direction = RIGHT;
    }
    else if ((right_sensor >= 3) && (left_sensor < 3))
    {
        robot->direction = LEFT;
    }
    else if ((left_sensor >= 3) && (right_sensor >= 3) && (front_centre_sensor == 0))
    {
        robot->direction = UP;
    }
    else if (front_centre_sensor >= 1 && robot->currentSpeed > 0)
    { // Wall incoming in front
        robot->turn_amount = 0;
        robot->turn_count = 0;
        robot->timer = 0;
        robot->direction = DOWN;
        robot->left_score = 0;
        robot->right_score = 0;
        robot->found_front_wall = 1;
        robot->count = 0;
        robot->direction = DOWN;
        robot->found_front_wall = 1;
    }
    else if (front_centre_sensor == 0 && robot->currentSpeed < 6)
    {
        robot->direction = UP;
    }
}