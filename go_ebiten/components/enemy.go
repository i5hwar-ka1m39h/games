package components

import (
	"image"
	"math"
	"math/rand"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

type Dragon struct {
	InitPos    gtypes.Vector
	Velocity   gtypes.Vector
	EnemySpeed float64

	Frames       []*ebiten.Image
	CurrentFrame int

	Timer          utils.Timer
	MovementTimer  utils.Timer
	AttactInterval utils.Timer
	Health         int
}

func NewDragon(
	initPos gtypes.Vector,
	speed float64,
	fullImage *ebiten.Image,
	animTime time.Duration,
	moveTime time.Duration,
	attactTime time.Duration,
) *Dragon {

	imgWidth := fullImage.Bounds().Dx()
	imgHeight := fullImage.Bounds().Dy()

	singleWidth := imgWidth / 3
	singleHeight := imgHeight / 4

	frames := []*ebiten.Image{}

	row := 2
	for col := 0; col < 3; col++ {
		rect := image.Rect(
			col*singleWidth,
			row*singleHeight,
			(col+1)*singleWidth,
			(row+1)*singleHeight,
		)

		frame := fullImage.SubImage(rect).(*ebiten.Image)
		frames = append(frames, frame)
	}

	d := &Dragon{
		InitPos:        initPos,
		EnemySpeed:     speed,
		Frames:         frames,
		CurrentFrame:   0,
		Timer:          *utils.NewTimer(animTime),
		MovementTimer:  *utils.NewTimer(moveTime),
		AttactInterval: *utils.NewTimer(attactTime),
		Health:         100,
	}

	// Give it an initial direction
	d.ChangeDirection()

	return d
}

func (d *Dragon) ReduceHealth() {
	d.Health--
}

func (d *Dragon) IncreaseHealth() {
	d.Health++
}
func (d *Dragon) ChangeDirection() {
	x := rand.Float64()*2 - 1
	y := rand.Float64()*2 - 1

	length := math.Sqrt(x*x + y*y)

	// Avoid division by zero
	if length == 0 {
		x = 1
		y = 0
		length = 1
	}

	d.Velocity.X = x / length
	d.Velocity.Y = y / length
}

func (d *Dragon) UpdateEnemy(screenWidth, screenHeight int) {

	//------------------------------------------------
	// Animate sprite
	//------------------------------------------------

	d.Timer.Update()

	if d.Timer.IsReady() {
		d.CurrentFrame++

		if d.CurrentFrame >= len(d.Frames) {
			d.CurrentFrame = 0
		}

		d.Timer.Reset()
	}

	//------------------------------------------------
	// Change direction every few seconds
	//------------------------------------------------

	d.MovementTimer.Update()

	if d.MovementTimer.IsReady() {
		d.ChangeDirection()
		d.MovementTimer.Reset()
	}

	//------------------------------------------------
	// Move every frame
	//------------------------------------------------

	d.InitPos.X += d.Velocity.X * d.EnemySpeed
	d.InitPos.Y += d.Velocity.Y * d.EnemySpeed

	//------------------------------------------------
	// Bounce from walls
	//------------------------------------------------
	left := 0.0
	right := float64(screenWidth - d.Frames[d.CurrentFrame].Bounds().Dx())

	top := 0.0
	bottom := float64(screenHeight)/2 -
		float64(d.Frames[d.CurrentFrame].Bounds().Dy())

	if d.InitPos.X < left {
		d.InitPos.X = left
		d.Velocity.X *= -1
	}

	if d.InitPos.X > right {
		d.InitPos.X = right
		d.Velocity.X *= -1
	}

	if d.InitPos.Y < top {
		d.InitPos.Y = top
		d.Velocity.Y *= -1
	}

	if d.InitPos.Y > bottom {
		d.InitPos.Y = bottom
		d.Velocity.Y *= -1
	}
}

func (d *Dragon) DrawEnemy(screen *ebiten.Image) {
	op := &ebiten.DrawImageOptions{}

	op.GeoM.Translate(d.InitPos.X, d.InitPos.Y)

	screen.DrawImage(d.Frames[d.CurrentFrame], op)
}

func (d *Dragon) Collision() *utils.Rect {
	bounds := d.Frames[d.CurrentFrame].Bounds()
	return utils.NewRect(d.InitPos.X, d.InitPos.Y, float64(bounds.Dx()), float64(bounds.Dy()))
}
