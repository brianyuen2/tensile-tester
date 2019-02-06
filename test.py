import msvcrt

num = 0
done = False
while not done:
    print(num)
    num += 1

    if msvcrt.kbhit():
        print (msvcrt.getch())
        done = True