package main

import (
	"log"
	// "math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/components"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
)

var wndSz = gtypes.Dimension{
	Width:  960,
	Height: 640,
}
var playersprite = assets.PlayerImage
var bgSpite = assets.BGImg

var player = components.NewPlayer(gtypes.Vector{
	X: float64(wndSz.Width) * 0.5,
	Y: float64(wndSz.Height) * 0.8,
}, 5.0, playersprite)

var background = components.NewBackground(bgSpite, 2.0)

// this is base struct that in need in ebiten.RunGame
type Game struct {
	PlayerPos  gtypes.Vector
	WindowSize gtypes.Dimension
}

// update logic
func (game *Game) Update() error {
	background.UpdateBackground()
	player.UpdatePlayer()

	return nil
}

// for drawing shit on screen
func (game *Game) Draw(screen *ebiten.Image) {
	background.DrawBackground(screen, game.WindowSize.Width)
	player.DrawPlayer(screen)

}

func (game *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 960, 640
}
func main() {

	game := &Game{
		PlayerPos: gtypes.Vector{
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
