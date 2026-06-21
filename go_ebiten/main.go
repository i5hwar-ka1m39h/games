package main

import (
	"log"
	// "math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
)

type Vector struct {
	X float64
	Y float64
}

type Dimensions struct {
	Width  int
	Height int
}

// this is base struct that in need in ebiten.RunGame
type Game struct {
	PlayerPos  Vector
	WindowSize Dimensions
}

// update logic
func (game *Game) Update() error {
	speed := float64(300 / ebiten.TPS())

	if ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
		game.PlayerPos.Y -= speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
		game.PlayerPos.Y += speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
		game.PlayerPos.X -= speed
	}

	if ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
		game.PlayerPos.X += speed
	}
	return nil
}

// for drawing shit on screen
func (game *Game) Draw(scree *ebiten.Image) {
	op := &ebiten.DrawImageOptions{} //how image should look

	//op.GeoM.Scale(0.5, 0.5)                                                // size adding (1, -1) rotates the image
	plyrImgWidth := float64(assets.PlayerImage.Bounds().Dx())
	plyrImgHeight := float64(assets.PlayerImage.Bounds().Dy())

	hlWidth := float64(plyrImgWidth / 2)
	hlHeight := float64(plyrImgHeight / 2)

	// op.GeoM.Rotate(math.Pi)  // it takes radians not so keep it mind also keep in mind the screen and image relation this can fuck up and display image in negative part
	op.GeoM.Translate(game.PlayerPos.X-hlWidth, game.PlayerPos.Y-hlHeight) //move the image

	scree.DrawImage(assets.PlayerImage, op)
}

func (game *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 960, 640
}
func main() {

	wndSz := Dimensions{
		Width:  960,
		Height: 640,
	}
	game := &Game{
		PlayerPos: Vector{
			X: float64(wndSz.Width) * 0.5,
			Y: float64(wndSz.Height) * 0.8,
		},
		WindowSize: wndSz,
	}

	ebiten.SetWindowSize(game.WindowSize.Width, game.WindowSize.Height)
	ebiten.SetWindowTitle("Gutterize")

	if err := ebiten.RunGame(game); err != nil {
		log.Fatalln("error starting the game", err)
	}

}
