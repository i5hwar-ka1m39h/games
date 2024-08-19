import tkinter
import random

#TILE SPECS
TILE_HEIGHT = 25
TILE_WIDTH = 25
TILE_SIZE = 25

#WIDNOW SPECS
TOTAL_COLUMNS = 25
TOTAL_ROWS = 25

WINDOW_HEIGHT = TILE_HEIGHT * TOTAL_ROWS
WINDOW_WIDTH = TILE_WIDTH * TOTAL_COLUMNS


#tile class
class Tile:
    def __init__(self,x,y):
        self.x = x
        self.y = y

#window define
window = tkinter.Tk()
window.title('Snakes')
window.resizable(False, False)

#define the canvas
canvas = tkinter.Canvas(window, bg= 'black', width= WINDOW_WIDTH, height= WINDOW_HEIGHT, highlightthickness= 0, borderwidth= 0)
canvas.pack()


#updating the window so canvas changes get added
window.update()

#fixing the game window
game_window_height = window.winfo_height()
game_window_width = window.winfo_width()
screen_height = window.winfo_screenheight()
screen_width = window.winfo_screenwidth()

#top left will be (0,0) and the botton right (screen_width, screen_height)
#so top of game window will be at point(screen_width/2 - game_width/2, screen_height/2 - game_height/2)
win_x = int((screen_width /2) - (game_window_width /2))
win_y = int((screen_height /2) - (game_window_height /2))

window.geometry(f'{game_window_height}x{game_window_width}+{win_x}+{win_y}')

#initialising the game
snake = Tile(5*TILE_WIDTH, 5*TILE_HEIGHT) 
food = Tile(10*TILE_WIDTH, 10*TILE_HEIGHT) 
snake_body = []  #collection of tiles that are going to be added 
velocityX = 0
velocityY = 0
game_over = False
score = 0

def change_direction(e):
    global velocityX, velocityY, game_over

    if game_over:
        return
    

    if (e.keysym == 'Up' and velocityY != 1):
        velocityX = 0
        velocityY = -1

    elif (e.keysym == 'Down' and velocityY != -1):
        velocityX = 0
        velocityY = 1

    elif (e.keysym == 'Right' and velocityX != -1):
        velocityX = 1
        velocityY = 0

    elif (e.keysym == 'Left' and velocityX != 1):
        velocityX = -1
        velocityY = 0
    
def move():
    global snake, food, snake_body, game_over, score

    if game_over:
        return
    
    if ( snake.x < 0 or snake.x >= WINDOW_WIDTH or snake.y < 0 or snake.y >= WINDOW_HEIGHT):
        game_over = True
        return
    
    for tile in snake_body:
        if(snake.x == tile.x and snake.y == tile.y):
            game_over = True
            return

    #eat food
    if ( food.x == snake.x and food.y == snake.y):
        snake_body.append(Tile(food.x, food.y))
        food.x = random.randint(0, TOTAL_COLUMNS -1 ) * TILE_SIZE
        food.x = random.randint(0, TOTAL_ROWS -1 ) * TILE_SIZE
        score += 1

    #updating the snake body
    for i in range(len(snake_body)-1, -1, -1):
        tile = snake_body[i]

        if (i== 0):
            tile.x = snake.x
            tile.y = snake.y

        else:
            prev_tile = snake_body[i -1]
            tile.x = prev_tile.x
            tile.y = prev_tile.y

    snake.x += velocityX * TILE_SIZE
    snake.y += velocityY * TILE_SIZE 

def draw():
    global snake, food, snake_body, score, game_over

    move()

    canvas.delete('all')

    
    #creating food
    canvas.create_rectangle(food.x, food.y, food.x+TILE_WIDTH, food.y+TILE_HEIGHT, fill='red')

    #creating snake
    canvas.create_rectangle(snake.x, snake.y, snake.x+TILE_WIDTH, snake.y+TILE_HEIGHT, fill='green')

    for single_tile in snake_body:
        canvas.create_rectangle(single_tile.x, single_tile.y, single_tile.x+TILE_SIZE, single_tile.y+TILE_SIZE, fill='green' )

    if(game_over):
        canvas.create_text(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, font='Arial 20', text=f"Game Over: {score}", fill='white')
    else:
        canvas.create_text(30, 20, font='Arial 10', text=f"Score: {score}", fill='white')

    window.after(100,draw)

draw()


window.bind('<KeyRelease>', change_direction)

#keep the window on
window.mainloop()