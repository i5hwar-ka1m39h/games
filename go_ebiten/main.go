package main

import (
	"fmt"
	"image/color"
	"log"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/assets"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/components"
	gtypes "github.com/i5hwar-ka1m39h/games/go_ebiten/g_types"
	"github.com/i5hwar-ka1m39h/games/go_ebiten/utils"
	"golang.org/x/image/font"
	"golang.org/x/image/font/basicfont"
	"golang.org/x/image/font/gofont/goregular"
	"golang.org/x/image/font/opentype"
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

var enemy *components.Dragon
var player *components.Player
var background *components.Background

type GameState int

const (
	StateStartScreen GameState = iota
	StatePlaying
	StatePlayerWon
	StatePlayerLost
)

const (
	startFadeIn    = 1500 * time.Millisecond
	startHold      = 1000 * time.Millisecond
	startFadeOut   = 1500 * time.Millisecond
	endScreenDelay = 3 * time.Second
)

var bigFace font.Face

func init() {
	tt, err := opentype.Parse(goregular.TTF)
	if err != nil {
		log.Fatal(err)
	}
	bigFace, err = opentype.NewFace(tt, &opentype.FaceOptions{
		Size:    36,
		DPI:     72,
		Hinting: font.HintingFull,
	})
	if err != nil {
		log.Fatal(err)
	}
}

type Game struct {
	PlayerPos  gtypes.Vector
	WindowSize gtypes.Dimension
	Bullets    []*components.Bullet
	FireBalls  []*components.FireBall
	Explosion  []*components.Explosion
	state      GameState
	stateStart time.Time
	startAlpha float64
	overlayImg *ebiten.Image
}

func resetGame() *Game {
	enemy = components.NewDragon(gtypes.Vector{
		X: float64(wndSz.Width) * 0.5,
		Y: float64(wndSz.Height) * 0.05,
	}, 2.0, dragonSprite, 150*time.Millisecond, 2*time.Second, 500*time.Millisecond)

	player = components.NewPlayer(gtypes.Vector{
		X: float64(wndSz.Width) * 0.5,
		Y: float64(wndSz.Height) * 0.8,
	}, 5.0, playersprite, 500*time.Millisecond)

	background = components.NewBackground(bgSpite, 2.0)

	return &Game{
		PlayerPos:  player.InitPos,
		WindowSize: wndSz,
		state:      StateStartScreen,
		stateStart: time.Now(),
		Bullets:    []*components.Bullet{},
		FireBalls:  []*components.FireBall{},
		Explosion:  []*components.Explosion{},
		overlayImg: ebiten.NewImage(wndSz.Width, wndSz.Height),
	}
}

func (game *Game) Update() error {
	switch game.state {
	case StateStartScreen:
		return game.updateStartScreen()
	case StatePlaying:
		return game.updatePlaying()
	case StatePlayerWon, StatePlayerLost:
		return game.updateEndScreen()
	}
	return nil
}

func (game *Game) updateStartScreen() error {
	elapsed := time.Since(game.stateStart)

	switch {
	case elapsed < startFadeIn:
		game.startAlpha = float64(elapsed) / float64(startFadeIn)
	case elapsed < startFadeIn+startHold:
		game.startAlpha = 1.0
	case elapsed < startFadeIn+startHold+startFadeOut:
		fadeProgress := float64(elapsed-startFadeIn-startHold) / float64(startFadeOut)
		game.startAlpha = 1.0 - fadeProgress
	default:
		game.state = StatePlaying
	}

	if ebiten.IsKeyPressed(ebiten.KeySpace) {
		game.state = StatePlaying
	}

	return nil
}

func (game *Game) updatePlaying() error {
	background.UpdateBackground()

	if !player.Finished {
		player.UpdatePlayer()
		plyrImgW := float64(player.ImgSprt.Bounds().Dx())
		plyrImgH := float64(player.ImgSprt.Bounds().Dy())
		halfW := plyrImgW * 1.5 / 2
		halfH := plyrImgH * 1.5 / 2
		player.InitPos.X = max(halfW, min(float64(wndSz.Width)-halfW, player.InitPos.X))
		player.InitPos.Y = max(halfH, min(float64(wndSz.Height)-halfH, player.InitPos.Y))
	}

	if !enemy.Finished {
		enemy.UpdateEnemy(wndSz.Width, wndSz.Height)
	}

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

	if !enemy.Finished {
		enemy.AttactInterval.Update()
		if enemy.AttactInterval.IsReady() {
			enemy.AttactInterval.Reset()

			fireBalltimer := utils.NewTimer(200 * time.Millisecond)
			fireBall := components.NewFireball(fireballPos, firballSprite, 8.00, *fireBalltimer)
			game.FireBalls = append(game.FireBalls, fireBall)
		}
	}

	for _, fireball := range game.FireBalls {
		fireball.Update()
	}

	for _, b := range game.Bullets {
		if !b.IsAlive {
			continue
		}
		for _, fb := range game.FireBalls {
			if !fb.IsAlive {
				continue
			}
			bRect := b.Collision()
			fbRect := fb.Collision()
			if bRect.Intersect(*fbRect) {
				b.IsAlive = false
				fb.IsAlive = false
				expX := (max(bRect.X, fbRect.X) + min(bRect.MaxX(), fbRect.MaxX())) / 2
				expY := (max(bRect.Y, fbRect.Y) + min(bRect.MaxY(), fbRect.MaxY())) / 2
				explosion := components.NewExplosion(gtypes.Vector{X: expX, Y: expY}, 1.0)
				game.Explosion = append(game.Explosion, explosion)
				break
			}
		}
	}

	for _, b := range game.Bullets {
		if !b.IsAlive {
			continue
		}
		if !enemy.Finished && b.Collision().Intersect(*enemy.Collision()) {
			b.IsAlive = false
			enemy.ReduceHealth()
		}
	}

	for _, fb := range game.FireBalls {
		if !fb.IsAlive {
			continue
		}
		if !player.Finished && fb.Collision().Intersect(*player.Collision()) {
			fb.IsAlive = false
			player.ReduceHealth()
		}
	}

	activeExplosions := []*components.Explosion{}
	for _, e := range game.Explosion {
		e.Update()
		if !e.Finished {
			activeExplosions = append(activeExplosions, e)
		}
	}
	game.Explosion = activeExplosions

	activeBullets := []*components.Bullet{}
	for _, b := range game.Bullets {
		if b.IsAlive {
			activeBullets = append(activeBullets, b)
		}
	}
	game.Bullets = activeBullets

	activeFireballs := []*components.FireBall{}
	for _, fb := range game.FireBalls {
		if fb.IsAlive {
			activeFireballs = append(activeFireballs, fb)
		}
	}
	game.FireBalls = activeFireballs

	if player.Health <= 0 && !player.Finished {
		player.Finished = true
		explosion := components.NewExplosion(player.InitPos, 4.0)
		game.Explosion = append(game.Explosion, explosion)
		game.state = StatePlayerLost
		game.stateStart = time.Now()
	} else if enemy.Health <= 0 && !enemy.Finished {
		enemy.Finished = true
		explosion := components.NewExplosion(enemy.InitPos, 4.0)
		game.Explosion = append(game.Explosion, explosion)
		game.state = StatePlayerWon
		game.stateStart = time.Now()
	}

	return nil
}

func (game *Game) updateEndScreen() error {
	if time.Since(game.stateStart) > endScreenDelay && ebiten.IsKeyPressed(ebiten.KeySpace) {
		newGame := resetGame()
		*game = *newGame
	}
	return nil
}

func (game *Game) Draw(screen *ebiten.Image) {
	switch game.state {
	case StateStartScreen:
		game.drawStartScreen(screen)
	case StatePlaying:
		game.drawPlaying(screen)
	case StatePlayerWon:
		game.drawPlaying(screen)
		game.drawEndScreen(screen, "YOU WIN!", color.RGBA{46, 204, 113, 255})
	case StatePlayerLost:
		game.drawPlaying(screen)
		game.drawEndScreen(screen, "GAME OVER", color.RGBA{231, 76, 60, 255})
	}
}

func (game *Game) drawStartScreen(screen *ebiten.Image) {
	screen.Fill(color.RGBA{0, 0, 0, 255})

	op := &ebiten.DrawImageOptions{}
	op.ColorScale.ScaleAlpha(float32(game.startAlpha))

	img := assets.StartScreenImg
	imgW := float64(img.Bounds().Dx())
	imgH := float64(img.Bounds().Dy())

	scaleX := float64(game.WindowSize.Width) / imgW
	scaleY := float64(game.WindowSize.Height) / imgH

	op.GeoM.Scale(scaleX, scaleY)
	screen.DrawImage(img, op)

	msg := "Press SPACE to start"
	msgBounds, _ := font.BoundString(bigFace, msg)
	msgWidth := (msgBounds.Max.X - msgBounds.Min.X).Ceil()
	text.Draw(screen, msg, bigFace,
		game.WindowSize.Width/2-msgWidth/2,
		game.WindowSize.Height-60,
		color.White)
}

func (game *Game) drawEndScreen(screen *ebiten.Image, message string, msgColor color.Color) {
	game.overlayImg.Fill(color.RGBA{0, 0, 0, 180})
	screen.DrawImage(game.overlayImg, nil)

	msgBounds, _ := font.BoundString(bigFace, message)
	msgWidth := (msgBounds.Max.X - msgBounds.Min.X).Ceil()
	text.Draw(screen, message, bigFace,
		game.WindowSize.Width/2-msgWidth/2,
		game.WindowSize.Height/2-20,
		msgColor)

	restartText := "Press SPACE to play again"
	restartBounds, _ := font.BoundString(bigFace, restartText)
	restartWidth := (restartBounds.Max.X - restartBounds.Min.X).Ceil()
	text.Draw(screen, restartText, bigFace,
		game.WindowSize.Width/2-restartWidth/2,
		game.WindowSize.Height/2+30,
		color.White)
}

func (game *Game) drawPlaying(screen *ebiten.Image) {
	background.DrawBackground(screen, game.WindowSize.Width)
	player.DrawPlayer(screen)
	enemy.DrawEnemy(screen)

	for _, bullet := range game.Bullets {
		bullet.Draw(screen)
	}

	for _, fireball := range game.FireBalls {
		fireball.Draw(screen)
	}

	for _, explosion := range game.Explosion {
		explosion.Draw(screen)
	}

	drawHealthBar(screen, 30, 30, 250, 16, player.Health, 100, "PLAYER HEALTH", color.RGBA{46, 204, 113, 255}, color.RGBA{46, 204, 113, 255})

	drawHealthBar(screen, 680, 30, 250, 16, enemy.Health, 100, "DRAGON HEALTH", color.RGBA{231, 76, 60, 255}, color.RGBA{231, 76, 60, 255})
}

func drawHealthBar(screen *ebiten.Image, x, y, width, height float32, current, max int, label string, labelColor color.Color, barColor color.Color) {
	displayVal := current
	if displayVal < 0 {
		displayVal = 0
	} else if displayVal > max {
		displayVal = max
	}

	borderColor := barColor
	vector.FillRect(screen, x, y, width, height, borderColor, false)

	bgBarColor := color.RGBA{20, 20, 20, 255}
	vector.FillRect(screen, x+4, y+4, width-8, height-8, bgBarColor, false)

	healthPct := float32(displayVal) / float32(max)
	fillWidth := (width - 8) * healthPct

	if fillWidth > 0 {
		vector.FillRect(screen, x+4, y+4, fillWidth, height-8, barColor, false)
	}

	face := basicfont.Face7x13

	text.Draw(screen, label, face, int(x), int(y)-8, labelColor)

	numText := fmt.Sprintf("%d/%d", displayVal, max)
	textWidth := len(numText) * 14
	text.Draw(screen, numText, face, int(x+width)-textWidth, int(y)-8, color.White)
}

func (game *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return 960, 640
}

func main() {
	game := resetGame()

	ebiten.SetWindowSize(game.WindowSize.Width, game.WindowSize.Height)
	ebiten.SetWindowTitle("Rulers of the Sky")

	if err := ebiten.RunGame(game); err != nil {
		log.Fatalln("error starting the game", err)
	}
}
