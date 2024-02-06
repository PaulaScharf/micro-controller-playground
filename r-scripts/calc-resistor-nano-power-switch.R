# This script fits a sqrt function on the given time-resistor pairs for the TPL5110
# cool website to calc resistor pairs from that value: http://mustcalculate.com/electronics/resistorfinder.php?r=573490&es=E96

x = c(30,60,180,600,1200,1800,3000,3600,5400,7200)
y = c(16.2,22,34.73,57.44,77.57,93.1,115.33,124,149.39,169)

fit = lm(y ~ sqrt(x))

plot(x, y,xlim = c(1,86500),ylim=c(1,800))
#plot(x, y)
lines(seq(1,86400), predict(fit,data.frame(x = seq(1,86400))), col='red') 
new_x <- 86400

# Predict new y value
predicted_y <- predict(fit, data.frame(x = new_x))

points(new_x, predicted_y, col = "blue", lwd = 5)

