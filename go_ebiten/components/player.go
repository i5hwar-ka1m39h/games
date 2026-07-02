package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
)

type Player struct {
	InitPos gtypes.Vector
	Speed   float64
	ImgSprt *ebiten.Image
}

func NewPlayer(initPos gtypes.Vector, speed float64, img *ebiten.Image) *Player {
	return &Player{
		InitPos: initPos,
		Speed:   speed,
		ImgSprt: img,
	}
}
func (plyr *Player) UpdatePlayer() error {
	speed := plyr.Speed

	if ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
		plyr.InitPos.Y -= speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
		plyr.InitPos.Y += speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
		plyr.InitPos.X -= speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
		plyr.InitPos.X += speed
	}
	return nil
}

func (plyr *Player) DrawPlayer(screen *ebiten.Image) {
	op := &ebiten.DrawImageOptions{} //how image should look

	op.GeoM.Scale(1.5, 1.5) // size adding (1, -1) rotates the image
	plyrImgWidth := float64(plyr.ImgSprt.Bounds().Dx())
	plyrImgHeight := float64(plyr.ImgSprt.Bounds().Dy())

	hlWidth := float64(plyrImgWidth / 2)
	hlHeight := float64(plyrImgHeight / 2)

	// op.GeoM.Rotate(math.Pi)  // it takes radians not so keep it mind also keep in mind the screen and image relation this can fuck up and display image in negative part
	op.GeoM.Translate(plyr.InitPos.X-hlWidth, plyr.InitPos.Y-hlHeight) //move the image

	screen.DrawImage(plyr.ImgSprt, op)

}
