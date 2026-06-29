package utils

import (
	"time"

	"github.com/hajimehoshi/ebiten/v2"
)

type Timer struct {
	currentTick int
	targetTick  int
}

func NewTimer(d time.Duration) *Timer {
	return &Timer{
		currentTick: 0,
		targetTick:  int(d.Milliseconds()) * ebiten.TPS() / 1000,
	}
}

func (t *Timer) Update() {
	if t.currentTick < t.targetTick {
		t.currentTick++
	}
}

func (t *Timer) IsReady() bool {
	return t.currentTick >= t.targetTick
}

func (t *Timer) Reset() {
	t.currentTick = 0
}
