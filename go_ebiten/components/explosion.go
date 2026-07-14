package components

import (
	"image"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

type Explosion struct {
	InitPos      gtypes.Vector
	Frames       []*ebiten.Image
	CurrentFrame int
	Timer        *utils.Timer
	Finished     bool
	Scale        float64
}

func NewExplosion(initPos gtypes.Vector, scale float64) *Explosion {
	expSprite := assets.ExplosionImg

	var frames []*ebiten.Image
	width := expSprite.Bounds().Dx()
	height := expSprite.Bounds().Dy()

	singleWidth := width / 3
	singleHeight := height / 3

	row := 1
	for col := 0; col < 2; col++ {

		rect := image.Rect(col*singleWidth, row*singleHeight, (col+1)*singleWidth, (row+1)*singleHeight)

		frame := expSprite.SubImage(rect).(*ebiten.Image)
		frames = append(frames, frame)

	}

	return &Explosion{
		InitPos:      initPos,
		Frames:       frames,
		CurrentFrame: 0,
		Timer:        utils.NewTimer(50 * time.Millisecond),
		Finished:     false,
		Scale:        scale,
	}

}

func (exp *Explosion) Update() error {
	if exp.Finished {
		return nil
	}
	exp.Timer.Update()

	if exp.Timer.IsReady() {
		exp.CurrentFrame++
		if exp.CurrentFrame >= len(exp.Frames) {
			exp.Finished = true

			return nil
		}

		exp.Timer.Reset()
	}

	return nil
}

func (exp *Explosion) Draw(screen *ebiten.Image) {
	if exp.Finished {
		return
	}
	op := ebiten.DrawImageOptions{}

	halfImgWidth := float64(exp.Frames[exp.CurrentFrame].Bounds().Dx()) / 2
	halfImgHeight := float64(exp.Frames[exp.CurrentFrame].Bounds().Dy()) / 2

	op.GeoM.Translate(-halfImgWidth, -halfImgHeight)
	op.GeoM.Scale(exp.Scale, exp.Scale)
	op.GeoM.Translate(exp.InitPos.X, exp.InitPos.Y)

	screen.DrawImage(exp.Frames[exp.CurrentFrame], &op)
}
