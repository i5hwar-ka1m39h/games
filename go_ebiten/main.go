package main

import (
	"log"
	"time"

	// "math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/components"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
)

var wndSz = gtypes.Dimension{
	Width:  960,
	Height: 640,
}
var playersprite = assets.PlayerImage
var bgSpite = assets.BGImg
var dragonSprite = assets.DragonImg
var bulletSprite = assets.BulletImg
var firballSprite = assets.FireballImg

var enemy = components.NewDragon(gtypes.Vector{
	X: float64(wndSz.Width) * 0.5,
	Y: float64(wndSz.Height) * 0.05,
}, 2.0, dragonSprite, 150*time.Millisecond, 2*time.Second, 2*time.Second)

var player = components.NewPlayer(gtypes.Vector{
	X: float64(wndSz.Width) * 0.5,
	Y: float64(wndSz.Height) * 0.8,
}, 5.0, playersprite, 150*time.Millisecond)

var background = components.NewBackground(bgSpite, 2.0)

// this is base struct that in need in ebiten.RunGame
type Game struct {
	PlayerPos  gtypes.Vector
	WindowSize gtypes.Dimension
	Bullets    []*components.Bullet
	FireBalls  []*components.FireBall
}

// update logic
func (game *Game) Update() error {
	background.UpdateBackground()
	player.UpdatePlayer()
	enemy.UpdateEnemy(wndSz.Width, wndSz.Height)

	enemyWidth := enemy.Frames[enemy.CurrentFrame].Bounds().Dx()
	enemyHeight := enemy.Frames[enemy.CurrentFrame].Bounds().Dy()

	fireballPos := gtypes.Vector{
		X: enemy.InitPos.X + float64(enemyWidth)/2.0,

		Y: enemy.InitPos.Y + float64(enemyHeight),
	}

	player.ShotCoolDown.Update()
	if player.WantToShoot() && player.ShotCoolDown.IsReady() {
		player.ShotCoolDown.Reset()

		bulletTimer := utils.NewTimer(150 * time.Millisecond)
		bullet := components.NewBullet(player.InitPos, bulletSprite, 5.0, bulletTimer)

		game.Bullets = append(game.Bullets, bullet)

	}

	for _, bullet := range game.Bullets {
		bullet.Update()
	}

	enemy.AttactInterval.Update()
	if enemy.AttactInterval.IsReady() {
		enemy.AttactInterval.Reset()

		fireBalltimer := utils.NewTimer(200 * time.Millisecond)
		fireBall := components.NewFireball(fireballPos, firballSprite, 8.00, *fireBalltimer)
		game.FireBalls = append(game.FireBalls, fireBall)
	}

	for _, fireball := range game.FireBalls {
		fireball.Update()
	}

	return nil
}

// for drawing shit on screen
func (game *Game) Draw(screen *ebiten.Image) {
	background.DrawBackground(screen, game.WindowSize.Width)
	player.DrawPlayer(screen)
	enemy.DrawEnemy(screen)

	for _, bullet := range game.Bullets {
		bullet.Draw(screen)
	}

	for _, fireball := range game.FireBalls {
		fireball.Draw(screen)
	}

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
