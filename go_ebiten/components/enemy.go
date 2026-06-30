package components

import (
	"image"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

type Dragon struct {
	InitPos      gtypes.Vector
	EnemySpeed   float64
	Frames       []*ebiten.Image
	CurrentFrame int
	Timer        utils.Timer
}

func NewDragon(initPos gtypes.Vector, speed float64, fullImage *ebiten.Image, endTime time.Duration) *Dragon {

	imgWidth := fullImage.Bounds().Dx()
	imgHeight := fullImage.Bounds().Dy()

	singleWidth := imgWidth / 3
	singleHeight := imgHeight / 4

	frames := []*ebiten.Image{}

	row := 2
	for col := 0; col < 3; col++ {

		rect := image.Rect(col*singleWidth, row*singleHeight, (col+1)*singleWidth, (row+1)*singleHeight)

		frame := fullImage.SubImage(rect).(*ebiten.Image)

		frames = append(frames, frame)
	}

	timer := utils.NewTimer(endTime)
	return &Dragon{
		InitPos:      initPos,
		EnemySpeed:   speed,
		Frames:       frames,
		Timer:        *timer,
		CurrentFrame: 0,
	}
}

func (d *Dragon) UpdateEnemy() error {

	d.Timer.Update()

	if d.Timer.IsReady() {
		d.CurrentFrame++

		if d.CurrentFrame >= len(d.Frames) {
			d.CurrentFrame = 0
		}
		d.Timer.Reset()
	}

	return nil
}

func (d *Dragon) DrawEnemy(screen *ebiten.Image) {
	op := ebiten.DrawImageOptions{}

	op.GeoM.Scale(2.0, 2.0)

	op.GeoM.Translate(d.InitPos.X, d.InitPos.Y)

	screen.DrawImage(d.Frames[d.CurrentFrame], &op)

}
