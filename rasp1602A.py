# 1.
# sudo apt-get install i2c-tools
# 2.
# sudo vim /etc/modules => 2줄 추가
# i2c-bcm2708
# i2c-dev

# 3.
# i2cdetect -y 1 
# 해당 LCD는 I2C 통신이란것을함
# I2C 통신을 하게 될 경우 각 기기마다 붙는 주소가 존재
# 27번 보이면 연결 된 것 

# 4.
# git clone https://github.com/the-raspberry-pi-guy/lcd.git
# cd lcd
# sudo sh install.sh => 한번 리부팅

# 리부팅 후 
# cd /home/pi/lcd/
# ./demo_clock.py =>