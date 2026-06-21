package main

import (
	"log"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
)

// this is base struct that in need in ebiten.RunGame
type Game struct{}

// update logic
func (game *Game) Update() error {
	return nil
}

// for drawing shit on screen
func (game *Game) Draw(scree *ebiten.Image) {
	op := &ebiten.DrawImageOptions{} //how image should look

	op.GeoM.Rotate(math.Pi)     // it takes radians not so keep it mind also keep in mind the screen and image relation this can fuck up and display image in negative part
	op.GeoM.Scale(0.5, 0.5)     // size adding (1, -1) rotates the image
	op.GeoM.Translate(150, 200) //move the image

	scree.DrawImage(assets.PlayerImage, op)
}

func (game *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 480, 320
}
func main() {

	game := &Game{}

	ebiten.SetWindowSize(960, 640)
	ebiten.SetWindowTitle("Gutterize")

	if err := ebiten.RunGame(game); err != nil {
		log.Fatalln("error starting the game", err)
	}

}
