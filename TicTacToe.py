import turtle
import random

# Module Subroutines

def draw_x(game_board: dict, board_position: str, writing_cursor: 'Turtle') -> None:
    """
    Draw an x on game_board at board_position using writing_cursor.
    """
    writing_cursor.goto(game_board[board_position][0][0], game_board[board_position][0][1])
    writing_cursor.pendown()
    writing_cursor.right(45)
    writing_cursor.forward(15)
    writing_cursor.backward(30)
    writing_cursor.forward(15)
    writing_cursor.left(90)
    writing_cursor.forward(15)
    writing_cursor.backward(30)
    writing_cursor.right(45)
    writing_cursor.penup()
    writing_cursor.goto(0, 0)

def draw_game_board(pencil: 'Turtle') -> None:
    """
    Draw a visual representation of a Tic Tac Toe Board.
    """
    pencil.pendown()
    pencil.left(90)
    pencil.forward(150)
    pencil.backward(50)
    pencil.left(90)
    pencil.forward(50)
    pencil.backward(150)
    pencil.forward(50)
    pencil.right(90)
    pencil.forward(50)
    pencil.backward(100)
    pencil.right(90)
    pencil.forward(50)
    pencil.backward(50)
    pencil.right(90)
    pencil.forward(50)
    pencil.backward(50)
    pencil.right(90)
    pencil.forward(100)
    pencil.right(90)
    pencil.penup()
    pencil.goto(0, 0)

def check_board_status(game_board: dict, board_condition: tuple, player: int) -> bool:
    """
    Return True if  and only if game_board is in board_condition such that a victory has occured for player.

    >>> game_board_ex = {'A1': [(-25.00, 125.00),'o'] ,'A2': [(25.00, 125.00), ''], 'A3': [(75.00, 125.00),''],
              'B1': [(-25.00, 75.00),'o'], 'B2': [(25.00, 75.00),''], 'B3': [(75.00, 75.00),''],
              'C1': [(-25.00, 25.00),'o'], 'C2': [(25.0, 25.00),''], 'C3': [(75.00, 25.00), '']}

    >>> check_board_status(game_board_ex, ('A1', 'B1', 'C1'))
    True

    >>> game_board_ex_2 = {'A1': [(-25.00, 125.00),'o'] ,'A2': [(25.00, 125.00), ''], 'A3': [(75.00, 125.00),''],
              'B1': [(-25.00, 75.00),'x'], 'B2': [(25.00, 75.00),''], 'B3': [(75.00, 75.00),''],
              'C1': [(-25.00, 25.00),'o'], 'C2': [(25.0, 25.00),''], 'C3': [(75.00, 25.00), '']}
    >>> check_board_status(game_board_ex_2, ('A1', 'B1', 'C1'))
    False
    """

    if player == 1:
        token = 'x'
    else:
        token = 'o'
    position_tokens = [game_board[position][1] for position in board_condition]
    return all([position_tokens[i] == token for i in range(len(position_tokens))])

# Create Pencil 

pencil = turtle.Turtle()
pencil.hideturtle()

# Game Board Representation.

game_board_rep = {'A1': [(-25.00, 125.00),''] ,'A2': [(25.00, 125.00), ''], 'A3': [(75.00, 125.00),''],
              'B1': [(-25.00, 75.00),''], 'B2': [(25.00, 75.00),''], 'B3': [(75.00, 75.00),''],
              'C1': [(-25.00, 25.00),''], 'C2': [(25.0, 25.00),''], 'C3': [(75.00, 25.00), '']}
game_board_positions = ['A1', 'A2', 'A3', 'B1', 'B2', 'B3', 'C1', 'C2', 'C3']
possible_winning_combinations = [('A1', 'B1', 'C1'), ('A2', 'B2', 'C2'), ('A3', 'B3', 'C3'), ('A1', 'A2', 'A3'), ('B1', 'B2', 'B3'), ('C1', 'C2', 'C3'),
                                         ('A1', 'B2', 'C3'), ('C1', 'B2', 'A3')]

# Choose The Number Of Players (And AI Difficulty)
player_mode = ''
victor = False
while player_mode != 'Q':
    pencil.goto(0.0, 0.0)
    draw_game_board(pencil)
    current_game_board = {}
    for key in game_board_rep:
        current_game_board[key] = game_board_rep[key]
        current_game_board[key][1] = ''
    player_mode = ''
    while player_mode != '1' and player_mode != '2' and player_mode != 'Q':
        player_mode = input('Type in a 1 or a 2 to specify the number of players, or Q to quit.')
    if player_mode == '1':
        AI_difficulty = input("Choose your opponent's difficulty!")
        while AI_difficulty not in ['EASY', 'MEDIUM', 'HARD', 'IMPOSSIBLE']:
            AI_difficulty = input('Please choose a difficulty from among the following: EASY, MEDIUM, HARD, IMPOSSIBLE')

    # Start A Single Player Game
    
        PLAYER_ONE = 1
        AI = 'Opponent'
        first_to_play = random.randint(1, 2)  # Determine Turn Order And First Player
        if first_to_play != 1:
            current_player = 'Opponent'
        else:
            current_player = first_to_play
        open_positions = game_board_positions.copy()
        while victor == False and len(open_positions) > 0:
            print("It is player " + str(current_player) + "'s turn!")
            if current_player == PLAYER_ONE:
                place_token = input('Select a position to place your token!')
                while place_token not in open_positions:
                    place_token = input('Select a position to place your token from the following positions:' + str(open_positions))
                open_positions.remove(place_token)
                draw_x(current_game_board, place_token, pencil)
                current_game_board[place_token][1] = 'x'
            else:
                if AI_difficulty == 'EASY':
                    place_token = open_positions[random.randint(0, len(open_positions) - 1)]
                elif AI_difficulty == 'MEDIUM':
                    pass
                elif AI_difficulty == 'HARD':
                    pass
                else:
                    pass
                open_positions.remove(place_token)
                pencil.goto(current_game_board[place_token][0])
                pencil.pendown()
                pencil.circle(10)
                pencil.penup()
                current_game_board[place_token][1] = 'o'

            # Check Victory Condition And Alter Current Player
            if any([check_board_status(current_game_board, combination, current_player) for combination in possible_winning_combinations]):
                victor = True
            else:
                if current_player == PLAYER_ONE:
                    current_player = AI
                else:
                    current_player = PLAYER_ONE

    # Start Two Player Game
    elif player_mode == '2':
        PLAYER_ONE = 1
        PLAYER_TWO = 2
        first_to_play = random.randint(1, 2)
        current_player = first_to_play
        open_positions = game_board_positions.copy()
        while victor == False and len(open_positions) > 0:
            print("It is player " + str(current_player) + "'s turn!")
            if current_player == PLAYER_ONE:
                place_token = input('Select a position to place your token!')
                while place_token not in open_positions:
                    place_token = input('Select a position to place your token from the following positions:' + str(open_positions))
                open_positions.remove(place_token)
                draw_x(current_game_board, place_token, pencil)
                current_game_board[place_token][1] = 'x'
            else:
                place_token = input('Select a position to place your token!')
                while place_token not in open_positions:
                    place_token = input('Select a position to place your token from the following positions:' + str(open_positions))
                open_positions.remove(place_token)
                pencil.goto(current_game_board[place_token][0])
                pencil.pendown()
                pencil.circle(10)
                pencil.penup()
                current_game_board[place_token][1] = 'o'
                
        # Check Victory Condition And Alter Current Player
            if any([check_board_status(current_game_board, combination, current_player) for combination in possible_winning_combinations]):
                victor = True
            else:
                if current_player == PLAYER_ONE:
                    current_player = PLAYER_TWO
                else:
                    current_player = PLAYER_ONE

    # End Game Declare Result, And Reset

    if victor == True:
        print('Player ' + str(current_player) + ' is the winner!')
        pencil.clear()
    else:
        print("It's a draw!")
        pencil.clear()
    pencil.right(90)
    victor = False
print('Goodbye!')





    


