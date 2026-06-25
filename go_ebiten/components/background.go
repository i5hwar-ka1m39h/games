package components

import "github.com/hajimehoshi/ebiten/v2"

type Background struct {
	Img      *ebiten.Image
	Speed    float64
	BGoffset float64
}

func NewBackground(img *ebiten.Image, speed float64) *Background {
	return &Background{
		Img:   img,
		Speed: speed,
	}
}

func (bg *Background) UpdateBackground() error {

	bg.BGoffset += bg.Speed

	bgHeight := float64(bg.Img.Bounds().Dy())

	if bg.BGoffset >= bgHeight {
		bg.BGoffset -= bgHeight
	}

	return nil
}

func (bg *Background) DrawBackground(screen *ebiten.Image, wndwWidth int) {

	bgHeight := float64(bg.Img.Bounds().Dy())
	bgWidth := float64(bg.Img.Bounds().Dx())

	scaleX := float64(wndwWidth) / bgWidth
	op1 := ebiten.DrawImageOptions{}
	op2 := ebiten.DrawImageOptions{}

	op1.GeoM.Scale(scaleX, 1)
	op2.GeoM.Scale(scaleX, 1)

	op1.GeoM.Translate(0, bg.BGoffset)
	op2.GeoM.Translate(0, bg.BGoffset-bgHeight)

	screen.DrawImage(bg.Img, &op1)
	screen.DrawImage(bg.Img, &op2)

}
