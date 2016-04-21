#include <ncurses.h>
#include <stdlib.h>

#include <time.h>

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct Room
{
    Position position;
    int height;
    int width;

    Position ** doors;
    // Monster ** monsters;
    // Item ** items;
} Room;

typedef struct Player
{
    Position position;
    int health;
} Player;

int screenSetup();
Room ** mapSetup();
Player * playerSetup();
int handleInput(int ch, Player * user);
int checkPosition(int newY, int newX, Player * user);
int playerMove(int y, int x, Player * user);

/* room functions */
Room * createRoom(int x, int y, int height, int width);
int drawRoom(Room * room);
int connectDoors(Position * doorsOne, Position * doorTwo);

int main()
{
    Player * user;
    int ch;

    screenSetup();

    mapSetup();

    user = playerSetup();

    while((ch = getch()) != 'q')
    {
        handleInput(ch, user);
    }

    endwin();

    return 0;
}


int screenSetup()
{
    initscr();
    printw("Hello world!");
    noecho();
    refresh();

    srand(time(NULL));


    return 1;
}

Room ** mapSetup()
{
    Room ** rooms;
    rooms = malloc(sizeof(Room) * 6);

    rooms[0] = createRoom(13, 13, 6, 8);
    drawRoom(rooms[0]);

    rooms[1] = createRoom(40, 2, 6, 8);
    drawRoom(rooms[1]);

    rooms[2] = createRoom(40, 10, 8, 12);

    drawRoom(rooms[2]);

    connectDoors(rooms[0]->doors[3], rooms[2]->doors[2]);
    connectDoors(rooms[1]->doors[2], rooms[0]->doors[0]);

    return rooms;
}

Room * createRoom(int x, int y, int height, int width)
{
    Room * newRoom;
    newRoom = malloc(sizeof(Room));

    newRoom->position.x = x;
    newRoom->position.y = y;
    newRoom->height = height;
    newRoom->width = width;

    newRoom->doors = malloc(4 * sizeof(Position));

    //top doors
    newRoom->doors[0] = malloc(sizeof(Position));
    newRoom->doors[0]->x = rand() % (width - 2) + newRoom->position.x + 1;
    newRoom->doors[0]->y = newRoom->position.y;

    //left doors
    newRoom->doors[1] = malloc(sizeof(Position));
    newRoom->doors[1]->y = rand() % (height - 2) + newRoom->position.y + 1;
    newRoom->doors[1]->x = newRoom->position.x;

    //bottom doors
    newRoom->doors[2] = malloc(sizeof(Position));
    newRoom->doors[2]->x = rand() % (width - 2) + newRoom->position.x + 1;
    newRoom->doors[2]->y = newRoom->position.y * newRoom->height - 1;

    //right doors
    newRoom->doors[3] = malloc(sizeof(Position));
    newRoom->doors[3]->y = rand() % (height - 2) + newRoom->position.y + 1;
    newRoom->doors[3]->x = newRoom->position.x + width - 1;

    return newRoom;
}

int drawRoom(Room * room)

{
    int x;
    int y;

    /* draw top and bottom */
    for(x = room->position.x; x < room->position.x + room->width; x++)
    {
        mvprintw(room->position.y, x, "-");
        mvprintw(room->position.y + room->height - 1, x, "-");
    }

    /* draw floors and sidewalls */
    for(y = room->position.y + 1; y < room->position.y + room->height - 1; y++)
    {
        //sidewalls
        mvprintw(y, room->position.x, "|");
        mvprintw(y, room->position.x + room->width - 1, "|");

        for(x = room->position.x + 1; x < room->position.x + room->width - 1; x++)
        {
            mvprintw(y, x, ".");
        }
    }

    /* draw doorss */
    mvprintw(room->doors[0]->y, room->doors[0]->x, "+");
    mvprintw(room->doors[1]->y, room->doors[1]->x, "+");
    mvprintw(room->doors[2]->y, room->doors[2]->x, "+");
    mvprintw(room->doors[3]->y, room->doors[3]->x, "+");

    return 1;
}

int connectDoors(Position * doorOne, Position * doorTwo)
{
    Position temp;
    Position previous;

    int count = 0;

    temp.x = doorOne->x;
    temp.y = doorOne->y;

    previous = temp;

    while(1)
    {
        // setp left
        if ((abs((temp.x - 1) - doorTwo->x) < abs(temp.x - doorTwo->x)) && (mvinch(temp.y, temp.x - 1) == ' '))
        {
            previous.x = temp.x;
            temp.x = temp.x - 1;

        } else if ((abs((temp.x + 1) - doorTwo->x) < abs(temp.x - doorTwo->x)) && (mvinch(temp.y, temp.x + 1) == ' '))
        {
            previous.x = temp.x;
            temp.x = temp.x + 1;

        } else if ((abs((temp.y + 1) - doorTwo->y) < abs(temp.y - doorTwo->y)) && (mvinch(temp.y + 1, temp.x) == ' '))
        {
            previous.y = temp.y;
            temp.y = temp.y + 1;
        } else if ((abs((temp.y - 1) - doorTwo->y) < abs(temp.y - doorTwo->y)) && (mvinch(temp.y - 1, temp.x) == ' '))
        {
            previous.y = temp.y;
            temp.y = temp.y - 1;
        } else {
            if (count == 0) {
                temp = previous;
                count++;
                continue;
            }

            return 0;
        }

        mvprintw(temp.y, temp.x, "#");

        getch();
    }

    return 1;
}

Player * playerSetup()
{
    Player * newPlayer;
    newPlayer = malloc(sizeof(Player));

    newPlayer->position.x = 14;
    newPlayer->position.y = 14;
    newPlayer->health = 20;

    playerMove(newPlayer->position.y, newPlayer->position.x, newPlayer);

    return newPlayer;
}

int handleInput(int input, Player * user)
{
    int newY;
    int newX;

    switch(input)
    {
        case 'w':
        case 'W':
            newY = user->position.y - 1;
            newX = user->position.x;
            break;

        case 's':
        case 'S':
            newY = user->position.y + 1;
            newX = user->position.x;
            break;

        case 'a':
        case 'A':
            newY = user->position.y;
            newX = user->position.x - 1;
            break;

        case 'd':
        case 'D':
            newY = user->position.y;
            newX = user->position.x + 1;
            break;
    }

    checkPosition(newY, newX, user);
}

int checkPosition(int newY, int newX, Player * user)
{
    int space;
    switch(mvinch(newY, newX))
    {
        case '.':
        case '#':
        case '+':
            playerMove(newY, newX, user);
            break;
        default:
            move(user->position.y, user->position.x);
            break;
    }

    return 1;
}

int playerMove(int y, int x, Player * user)
{
    mvprintw(user->position.y, user->position.x, ".");

    user->position.y = y;
    user->position.x = x;

    mvprintw(user->position.y, user->position.x, "@");
    move(user->position.y, user->position.x);
}
