package components

import (
	"image"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

type FireBall struct {
	InitPos      gtypes.Vector
	Frames       []*ebiten.Image
	Speed        float64
	CurrentFrame int
	Timer        utils.Timer
}

func NewFireball(initPos gtypes.Vector, img *ebiten.Image, speed float64, timer utils.Timer) *FireBall {

	imgWidth := img.Bounds().Dx()
	imgHeight := img.Bounds().Dy()

	eachWidth := imgWidth / 3
	eachHeight := imgHeight / 3

	frames := []*ebiten.Image{}
	row := 0

	for col := 0; col < 3; col++ {
		rect := image.Rect(
			col*eachWidth, row*eachHeight,
			(col+1)*eachWidth, (row+1)*eachHeight,
		)

		frame := img.SubImage(rect).(*ebiten.Image)

		frames = append(frames, frame)

	}

	return &FireBall{
		InitPos:      initPos,
		Frames:       frames,
		Speed:        speed,
		CurrentFrame: 0,
		Timer:        timer,
	}
}

func (fb *FireBall) Update() error {
	fb.Timer.Update()

	if fb.Timer.IsReady() {
		fb.CurrentFrame++

		if fb.CurrentFrame >= len(fb.Frames) {
			fb.CurrentFrame = 0
		}

		fb.Timer.Reset()
	}
	fb.InitPos.Y += fb.Speed
	return nil
}
func (fb *FireBall) Draw(screen *ebiten.Image) {
	op := &ebiten.DrawImageOptions{}

	scale := 0.2

	frame := fb.Frames[fb.CurrentFrame]

	width := float64(frame.Bounds().Dx())
	height := float64(frame.Bounds().Dy())

	// Move image origin to its center
	op.GeoM.Translate(-width/2, -height/2)

	// Scale
	op.GeoM.Scale(scale, scale)

	// Rotate 90 degrees clockwise
	op.GeoM.Rotate(math.Pi / 2)

	// Move to world position
	op.GeoM.Translate(
		fb.InitPos.X,
		fb.InitPos.Y,
	)

	screen.DrawImage(frame, op)
}
