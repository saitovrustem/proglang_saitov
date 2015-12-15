# -*- coding: utf-8 -*-
# Problem solutions for programming languages course
# Author: Rustem Saitov, 094 gr., FIVT

import time
import xml.dom.minidom
import floydmodule

# 2 resistors are connected in parallel - get total resistance
def parallel_resistance(r1, r2):
    return r1 * r2 / (r1 + r2)


# item_type = diode, capactor or resistor
# uses formula for parallel connection of elements
def process_edge(item, item_type, resistance_matrix):
        net_from = int(item.attributes['net_from'].value) - 1
        net_to = int(item.attributes['net_to'].value) - 1
        value = float(item.attributes['resistance'].value)

        if resistance_matrix[net_from][net_to] != 0:
            resistance_matrix[net_from][net_to] = parallel_resistance(resistance_matrix[net_from][net_to], value)
        else:
            resistance_matrix[net_from][net_to] = value

        if item_type == 'diode':
            value = float(item.attributes['reverse_resistance'].value)

        if resistance_matrix[net_to][net_from] != 0:
            resistance_matrix[net_to][net_from] = parallel_resistance(resistance_matrix[net_to][net_from], value)
        else:
            resistance_matrix[net_to][net_from] = value


def main():
    input_xml = open('input.xml', 'r')

    dom_tree = xml.dom.minidom.parse(input_xml)
    top_node = dom_tree._get_childNodes()[0]

    # Calculate number of nodes
    max_id = 0
    diode_elements = top_node.getElementsByTagName('diode')
    resistor_elements = top_node.getElementsByTagName('resistor')
    capactor_elements = top_node.getElementsByTagName('capactor')

    for item in diode_elements + resistor_elements + capactor_elements:
        max_id = max(max_id, int(item.attributes['net_from'].value))
        max_id = max(max_id, int(item.attributes['net_to'].value))

    # Build resistance matrix
    resistance_matrix = [[0 for i in range(0, max_id)] for j in range(0, max_id)]

    for item in top_node.getElementsByTagName('diode'):
        process_edge(item, 'diode', resistance_matrix)

    for item in top_node.getElementsByTagName('resistor'):
        process_edge(item, 'resistor', resistance_matrix)

    for item in top_node.getElementsByTagName('capactor'):
        process_edge(item, 'capactor', resistance_matrix)

    # Use cpp routine with fast floyd algorithm
    resistance_matrix = floydmodule.floyd_algorithm(resistance_matrix)

    # Output to csv file
    output_csv = open('output.csv', 'w')
    print(resistance_matrix)

    for line in resistance_matrix:
        output_csv.write(','.join(map(str, [line[i] for i in range(0, len(line))])))
        output_csv.write('\n')


if __name__ == '__main__':
    start = time.process_time()
    main()
    end = time.process_time()
    delta = end - start
    print("Algorithm work time: {:.1f} sec".format(delta))
