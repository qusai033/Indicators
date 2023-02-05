//@version=4
study(title="Humble LinReg Candles with UT Bot Alerts", shorttitle="LinReg Candles + UT Bot", format=format.price, precision=4, overlay=true)

signal_length = input(title="Signal Smoothing", type=input.integer, minval = 1, maxval = 200, defval = 11)
sma_signal = input(title="Simple MA (Signal Line)", type=input.bool, defval=true)

lin_reg = input(title="Lin Reg", type=input.bool, defval=true)
linreg_length = input(title="Linear Regression Length", type=input.integer, minval = 1, maxval = 200, defval = 11)

bopen = lin_reg ? linreg(open, linreg_length, 0) : open
bhigh = lin_reg ? linreg(high, linreg_length, 0) : high
blow = lin_reg ? linreg(low, linreg_length, 0) : low
bclose = lin_reg ? linreg(close, linreg_length, 0) : close

r = bopen < bclose

signal = sma_signal ? sma(bclose, signal_length) : ema(bclose, signal_length)

plotcandle(r ? bopen : na, r ? bhigh : na, r ? blow: na, r ? bclose : na, title="LinReg Candles", color= color.green, wickcolor=color.green, bordercolor=color.green, editable= true)
plotcandle(r ? na : bopen, r ? na : bhigh, r ? na : blow, r ? na : bclose, title="LinReg Candles", color=color.red, wickcolor=color.red, bordercolor=color.red, editable= true)

plot(signal, color=color.white)

// Inputs
a = input(1,     title = "Key Vaule. 'This changes the sensitivity'")
c = input(10,    title = "ATR Period")
h = input(false, title = "Signals from Heikin Ashi Candles")

xATR  = atr(c)
nLoss = a * xATR

src = h ? security(heikinashi(syminfo.tickerid), timeframe.period, close, lookahead = false) : close

xATRTrailingStop = 0.0
xATRTrailingStop := iff(src > nz(xATRTrailingStop[1], 0) and src[1] > nz(xATRTrailingStop[1], 0), max(nz(xATRTrailingStop[1]), src - nLoss),
   iff(src < nz(xATRTrailingStop[1], 0) and src[1] < nz(xATRTrailingStop[1], 0), min(nz(xATRTrailingStop[1]), src + nLoss), 
   iff(src > nz(xATRTrailingStop[1], 0), src - nLoss, src + nLoss)))
 
pos = 0   
pos :=	iff(src[1] < nz(xATRTrailingStop[1], 0) and src > nz(xATRTrailingStop[1], 0), 1,
   iff(src[1] > nz(xATRTrailingStop[1], 0) and src < nz(xATRTrailingStop[1], 0), -1, nz(pos[1], 0))) 
   
xcolor = pos == -1 ? color.red: pos == 1 ? color.green : color.blue 

ema   = ema(src,1)
above = crossover(ema, xATRTrailingStop)
below = crossover(xATRTrailingStop, ema)

buy  = src > xATRTrailingStop and above 
sell = src < xATRTrailingStop and below

barbuy  = src > xATRTrailingStop 
barsell = src < xATRTrailingStop 

plotshape(buy,  title = "Buy",  text = 'Buy',  style = shape.labelup,   location = location.belowbar, color= color.green, textcolor = color.white, size = size.tiny)
plotshape(sell, title = "Sell", text = 'Sell', style = shape.labeldown, location = location.abovebar, color= color.red,   textcolor = color.white, size = size.tiny)

barcolor(barbuy  ? color.green : na)
barcolor(barsell ? color.red   : na)

alertcondition(buy,  "UT Long",  "UT Long")
alertcondition(sell, "UT Short", "UT Short")
