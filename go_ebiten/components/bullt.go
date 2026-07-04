package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

type Bullet struct {
	InitPos gtypes.Vector
	Sprite  *ebiten.Image
	Speed   float64
	Timer   *utils.Timer
	IsAlive bool
}

func NewBullet(initPos gtypes.Vector, img *ebiten.Image, speed float64, timer *utils.Timer) *Bullet {
	return &Bullet{
		InitPos: initPos,
		Sprite:  img,
		Speed:   speed,
		Timer:   timer,
		IsAlive: true,
	}
}

func (b *Bullet) Update() error {
	b.InitPos.Y -= b.Speed
	return nil
}

func (b *Bullet) Draw(screen *ebiten.Image) {

	op := ebiten.DrawImageOptions{}

	// bulWidth := b.Sprite.Bounds().Dx()
	bulHeight := b.Sprite.Bounds().Dy()

	op.GeoM.Translate(b.InitPos.X, b.InitPos.Y-float64(bulHeight))

	screen.DrawImage(b.Sprite, &op)
}
