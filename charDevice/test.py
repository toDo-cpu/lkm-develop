with open("/dev/chardev","a") as driver:
	driver.write("hello world !")

with open("/dev/chardev","r") as driver:
	print(driver.read())