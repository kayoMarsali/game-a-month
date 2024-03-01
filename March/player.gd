extends CharacterBody2D

class_name Player


@export var speed_multiplier := 1.0
@export var max_shots := 3

const SPEED := 250.0


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	if Input.is_action_just_pressed("player_fire"):
		print("Fire")
		
func _physics_process(_delta: float) -> void:
	move_player()
	

func move_player():
	var direction := Vector2.ZERO
	if Input.is_action_pressed("player_move_up"):
		direction.y += -1
	if Input.is_action_pressed("player_move_down"):
		direction.y += 1
	if Input.is_action_pressed("player_move_left"):
		direction.x += -1
	if Input.is_action_pressed("player_move_right"):
		direction.x += 1
	
	if(direction.length() > 0):
		if false == direction.is_normalized():
			direction = direction.normalized()
		velocity = direction * (SPEED * speed_multiplier)
		move_and_slide()
