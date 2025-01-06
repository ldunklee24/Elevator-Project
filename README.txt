In order to build an executable from my code I put all of the header files into the header section of visual studio, I put the txt files into the resource section of visual studio, and I put the .cpp files into the source section of visual studio. From there I just hit the local windows debugger and the code runs.


The features implemented in my elevator follow

1. Space Bar stops and resumes the simulation and a red bar appears next to the top of the elevator when stopped.

2. The elevator shows which floor the passengers are going to, represented by dots

3. Passengers are differentiated by color, I randomly chose a color different from the color of the elevator so as to not have the passenger blend in

4. There is a progress by that tracks time by filling up by a percentage relating to the number of people that will be picked up and dropped off every time a passenger is dropped off. For example, if there are 5 total passengers in the file the bar will fill up 1/5 every time it drops a passenger off.

5. The lights of the buttons on the floor light up when a passenger is waiting and once they are picked up the light turns off.

6. There are green boxes that represent the number of passengers currently on the elevator. For example if there are two passengers on there will be two green boxes 

Features I didn't implement follow

1. Text via the font file, so I don't have actual numbers that represent what floors people need to be dropped off on

Known bugs follow

1. I was never able to pass the specific 2_passenger_test_2 on gradescope so that specific test will not work.

2. The elevator doesn't always properly drop passengers off in the correct manner - depending on the input it may get a passenger going down but go up a floor to pick another passenger up before going down and dropping off the first passenger.