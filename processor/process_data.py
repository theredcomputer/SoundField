from serial import Serial
from math import sqrt


# Configuration values
SENSOR_PITCH = 12 # inches
FIELD_LENGTH = 15 * SENSOR_PITCH # 15ft
FIELD_WIDTH  = 10 * SENSOR_PITCH # 10ft
NUM_SENSORS  = 10

MAX_OBJ_WIDTH = 24
MAX_OBJ_SWAY = 10

COUNT_THRESH = 3
STRIKE_THRESH = 3


if __name__ == "__main__":

    ser0 = Serial('/dev/ttyACM0', 230400)
    #ser1 = Serial('/dev/ttyACM1', 230400)
    
    coords = []    
    tracked_coords = [] # Data format: [[(x,y), count, strikes], ...]

    while 1:
        coords = []
        dist_data_0 = map(int, ser0.readline().split())
        #dist_data_1 = map(int, ser1.readline().split())
        #dist_data_1 = [FIELD_LENGTH - d1 for d1 in dist_data_1]
        dist_data_1 = [FIELD_LENGTH - 140 for i in range(NUM_SENSORS)]
        
        print dist_data_0
        #print dist_data_1
        
        '''
            Detection process:
    
            First, check to see if the object is seen by both A and B in a column.
            Next, check to see if A sees anything (B doesn't see it).
            Next, check to see if B sees anything (A doesn't see it).
            
        '''
        # Get the sensor columns where an object is seen by both sensors
        seen_by_both = [i for i, e in enumerate([1 if abs(d0 - d1) < MAX_OBJ_WIDTH / 2 else 0 for d0, d1 in zip(dist_data_0, dist_data_1)]) if e != 0]
        
        # Translate seen items into coordinates
        coords.extend([(s * SENSOR_PITCH, d0 + (MAX_OBJ_WIDTH / 2)) for s, d0 in zip(seen_by_both, dist_data_0)])

        # Get anything seen by A
        #seen_by_a = [i for i, e in enumerate([1 if d0 > 0 else 0 for d0 in dist_data_0]) if e != 0]
        #coords.extend([(s * SENSOR_PITCH, d0 + (MAX_OBJ_WIDTH / 2)) for s, d0 in zip(seen_by_a, dist_data_0])
        

        # Get anything seen by B
        #seen_by_b = [i for i, e in enumerate([1 if d1 > 0 else 0 for d1 in dist_data_1]) if e != 0]
        #coords.extend([(s * SENSOR_PITCH, d1 + (MAX_OBJ_WIDTH / 2)) for s, d1 in zip(seen_by_b, dist_data_1])
        
        # Remove duplicate values
        tmp = []
        for c in coords:
            if c not in tmp:
                tmp.append(c)
        coords = tmp
        
        # To get more x-axis resolution, interpolate between nearby points
        new_points = []
        old_points = []
        for i in range(len(coords) - 1):
            for j in range(i+1, len(coords)):
                if abs(coords[i][0] - coords[j][0]) == SENSOR_PITCH and \
                   abs(coords[i][1] - coords[j][1]) < MAX_OBJ_SWAY:
                    new_points.append(((coords[i][0] if coords[i][0] < coords[j][0] else coords[j][0]) + SENSOR_PITCH / 2, (coords[i][1] if coords[i][1] < coords[j][1] else coords[j][1]) + abs(coords[i][1] - coords[j][1]) / 2))
                    old_points.append(coords[i])
                    old_points.append(coords[j])
        tmp = []
        for o in old_points:
            if o not in tmp:
                tmp.append(o)
        [coords.remove(t) for t in tmp]
        [coords.append(n) for n in new_points]
        coords.sort(key=lambda y: y[0])
        
        print coords
       
        # Add any coordinate which isn't currently being tracked
        for c in coords:
            index = 0
            if tracked_coords == []:
                tracked_coords.append([c, 1, 0])
                continue
            for t in tracked_coords:

                # Handle columns which have a tracked point in it
                if c[0] == t[0][0]:

                    # If the observed coordinate is close to a previously-observed point, add to the counter and reset strikes. Otherwise, add it to the list of tracked points.
                    if c[1] > t[0][1] - MAX_OBJ_SWAY / 2 and \
                       c[1] < t[0][1] + MAX_OBJ_SWAY / 2:
                        t[1] = t[1] + 1
                        t[2] = 0
                    else:
                        tracked_coords.insert(index, [c, 1, 0])
                    break
                else:
                    # We need to find where to insert a point in an untracked column
                    if c[0] > t[0][0]:
                        index = index + 1
                    else:
                        tracked_coords.insert(index, [c, 1, 0])
                        break
            if index == len(tracked_coords):
                tracked_coords.append([c, 1, 0])

        # Decay all points by adding a strike. If unseen for too long, evict.
        for t in tracked_coords:
            t[2] = t[2] + 1
            if t[2] > STRIKE_THRESH:
                tracked_coords.remove(t)

        # Serve points which are stable
        stable_coords = [t[0] for t in tracked_coords if t[1] > COUNT_THRESH]

        print tracked_coords
        print stable_coords
        print
