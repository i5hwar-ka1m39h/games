package main

import (
	"fmt"
	"image/color"
	"log"
	"time"

	// "math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/components"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
	"golang.org/x/image/font/basicfont"
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

	//collision logic

	//if Bullets touches FireBalls remove FireBall and Bullets from their respective arr
	for i, b := range game.Bullets {
		for j, fb := range game.FireBalls {
			if b.Collision().Intersect(*fb.Collision()) {

				game.Bullets = append(game.Bullets[:i], game.Bullets[i+1:]...)
				game.FireBalls = append(game.FireBalls[:j], game.FireBalls[j+1:]...)

			}
		}
	}

	//if bullet touches the dragon then we need to reduce its health

	for _, b := range game.Bullets {
		if b.Collision().Intersect(*enemy.Collision()) {
			enemy.ReduceHealth()
		}

	}

	//if firball touchs the plane then we reduce plane health

	for _, fb := range game.FireBalls {
		if fb.Collision().Intersect(*player.Collision()) {
			player.ReduceHealth()
		}
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

	// Draw Player Health Bar
	// Placed at Top Left (X: 30, Y: 30), Width: 250, Height: 16
	// Label: Green (#2ecc71), Fill: Green (#2ecc71)
	drawHealthBar(screen, 30, 30, 250, 16, player.Health, 100, "PLAYER HEALTH", color.RGBA{46, 204, 113, 255}, color.RGBA{46, 204, 113, 255})

	// Draw Enemy Health Bar
	// Placed at Top Right (X: 680, Y: 30), Width: 250, Height: 16
	// Label: Crimson Red (#e74c3c), Fill: Crimson Red (#e74c3c)
	drawHealthBar(screen, 680, 30, 250, 16, enemy.Health, 100, "DRAGON HEALTH", color.RGBA{231, 76, 60, 255}, color.RGBA{231, 76, 60, 255})
}

// drawHealthBar renders a stylized, premium health bar with a border, background, active progress, highlight shine, and text label
func drawHealthBar(screen *ebiten.Image, x, y float32, width, height float32, current, max int, label string, labelColor color.Color, barColor color.Color) {
	// Ensure health display values are clamped between 0 and max
	displayVal := current
	if displayVal < 0 {
		displayVal = 0
	} else if displayVal > max {
		displayVal = max
	}

	// 1. Draw outer border (dark grey)
	borderColor := color.RGBA{50, 50, 50, 220}
	vector.DrawFilledRect(screen, x, y, width, height, borderColor, false)

	// 2. Draw inner dark background (empty bar)
	bgBarColor := color.RGBA{20, 20, 20, 255}
	vector.DrawFilledRect(screen, x+2, y+2, width-4, height-4, bgBarColor, false)

	// 3. Draw active health progress
	healthPct := float32(displayVal) / float32(max)
	fillWidth := (width - 4) * healthPct

	if fillWidth > 0 {
		vector.DrawFilledRect(screen, x+2, y+2, fillWidth, height-4, barColor, false)

		// 4. Draw glossy shine/glare on the top half for a polished look
		glareColor := color.RGBA{255, 255, 255, 45}
		vector.DrawFilledRect(screen, x+2, y+2, fillWidth, (height-4)/2, glareColor, false)
	}

	// 5. Draw text label above the health bar
	face := basicfont.Face7x13
	
	// Label on the left
	text.Draw(screen, label, face, int(x), int(y)-8, labelColor)
	
	// Numeric value (e.g. "100/100") on the right
	numText := fmt.Sprintf("%d/%d", displayVal, max)
	textWidth := len(numText) * 7 // basicfont Face7x13 character width is 7px
	text.Draw(screen, numText, face, int(x+width)-textWidth, int(y)-8, color.White)
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
	ebiten.SetWindowTitle("Rulers of the Sky")

	if err := ebiten.RunGame(game); err != nil {
		log.Fatalln("error starting the game", err)
	}

}
