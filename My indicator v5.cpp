//@version=5
indicator(title='Humble LinReg Candles with UT Bot Alerts', shorttitle='LinReg Candles + UT Bot', format=format.price, precision=4, overlay=true)

signal_length = input.int(title='Signal Smoothing', minval=1, maxval=200, defval=11)
sma_signal = input(title='Simple MA (Signal Line)', defval=true)

lin_reg = input(title='Lin Reg', defval=true)
linreg_length = input.int(title='Linear Regression Length', minval=1, maxval=200, defval=11)

bopen = lin_reg ? ta.linreg(open, linreg_length, 0) : open
bhigh = lin_reg ? ta.linreg(high, linreg_length, 0) : high
blow = lin_reg ? ta.linreg(low, linreg_length, 0) : low
bclose = lin_reg ? ta.linreg(close, linreg_length, 0) : close

r = bopen < bclose

signal = sma_signal ? ta.sma(bclose, signal_length) : ta.ema(bclose, signal_length)

plotcandle(r ? bopen : na, r ? bhigh : na, r ? blow : na, r ? bclose : na, title='LinReg Candles', color=color.green, wickcolor=color.green, bordercolor=color.green, editable=true)
plotcandle(r ? na : bopen, r ? na : bhigh, r ? na : blow, r ? na : bclose, title='LinReg Candles', color=color.red, wickcolor=color.red, bordercolor=color.red, editable=true)

plot(signal, color=color.new(color.white, 0))

// Inputs
a = input(1, title='Key Vaule. \'This changes the sensitivity\'')
c = input(10, title='ATR Period')
h = input(false, title='Signals from Heikin Ashi Candles')

xATR = ta.atr(c)
nLoss = a * xATR

src = h ? request.security(ticker.heikinashi(syminfo.tickerid), timeframe.period, close, lookahead=barmerge.lookahead_off) : close

xATRTrailingStop = 0.0
iff_1 = src > nz(xATRTrailingStop[1], 0) ? src - nLoss : src + nLoss
iff_2 = src < nz(xATRTrailingStop[1], 0) and src[1] < nz(xATRTrailingStop[1], 0) ? math.min(nz(xATRTrailingStop[1]), src + nLoss) : iff_1
xATRTrailingStop := src > nz(xATRTrailingStop[1], 0) and src[1] > nz(xATRTrailingStop[1], 0) ? math.max(nz(xATRTrailingStop[1]), src - nLoss) : iff_2

pos = 0
iff_3 = src[1] > nz(xATRTrailingStop[1], 0) and src < nz(xATRTrailingStop[1], 0) ? -1 : nz(pos[1], 0)
pos := src[1] < nz(xATRTrailingStop[1], 0) and src > nz(xATRTrailingStop[1], 0) ? 1 : iff_3

xcolor = pos == -1 ? color.red : pos == 1 ? color.green : color.blue

ema = ta.ema(src, 1)
above = ta.crossover(ema, xATRTrailingStop)
below = ta.crossover(xATRTrailingStop, ema)

buy = src > xATRTrailingStop and above
sell = src < xATRTrailingStop and below

barbuy = src > xATRTrailingStop
barsell = src < xATRTrailingStop

plotshape(buy, title='Buy', text='Buy', style=shape.labelup, location=location.belowbar, color=color.new(color.green, 0), textcolor=color.new(color.white, 0), size=size.tiny)
plotshape(sell, title='Sell', text='Sell', style=shape.labeldown, location=location.abovebar, color=color.new(color.red, 0), textcolor=color.new(color.white, 0), size=size.tiny)

barcolor(barbuy ? color.green : na)
barcolor(barsell ? color.red : na)

alertcondition(buy, 'UT Long', 'UT Long')
alertcondition(sell, 'UT Short', 'UT Short')

