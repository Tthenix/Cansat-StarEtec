
import re
import os
import serial
import csv

# Ruta al directorio de datos relativa al directorio actual
data_directory = os.path.join(os.path.dirname(__file__), '..', 'data')

# Verificar si el directorio de datos existe, si no, créalo
if not os.path.exists(data_directory):
    os.makedirs(data_directory)

# Encontrar el próximo nombre de archivo disponible
def find_available_filename(base_filename, extension):
    index = 1
    while True:
        filename = f"{base_filename}_{index:02d}.{extension}"
        full_path = os.path.join(data_directory, filename)
        if not os.path.exists(full_path):
            return full_path
        index += 1

# Configurar el puerto serial
ser = serial.Serial('COM11', baudrate=115200, timeout=1)

# Patrón para encontrar líneas de datos en el formato [i]:valor
data_pattern = re.compile(r'\[(\d+)\]:(\d+\.\d+)')

# Encontrar el próximo nombre de archivo disponible para CSV
csv_filename = find_available_filename("datos", "csv")

# Abrir el archivo CSV para escritura
with open(csv_filename, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';')  # Cambio del delimitador
 
    
    # Escribir encabezados de columnas
    # column_headers = [f'Dato_{i}' for i in range(19)]
    column_headers = [
        "mq7Pin",
        "mq7VoltageValue",
        "mq7COppm",
        "mAh",
        "vbat",
        "vaps",
        "icharge",
        "idischarge",
        "tempAXP192",
        "dhtHum",
        "dhtTemp",
        "dhtTempF",
        "bmpPres",
        "bmpTemp",
        "bmpAlti",
        "TFdistanceAverage",
        "TFstrength",
        "gpsLatitude",
        "gpsLongitude",
        "gpsAltitude"
        ]
    csv_writer.writerow(column_headers)
    
    # Bucle principal para leer datos del puerto serial
    while True:
        raw_line = ser.readline()
        try:
            line = raw_line.decode('latin-1').strip()
            print("Recibido:", line)
        except UnicodeDecodeError:
            print("Error de decodificación:", raw_line)
            continue
        
        # Verificar si la línea contiene el encabezado "Received packet Vector de datos"
        if line == "Received packet Vector de datos":
            data_values = [0.0] * 19  # Lista para almacenar los valores
            
            # Leer las siguientes 19 líneas con datos
            for _ in range(19):
                raw_data_line = ser.readline()
                try:
                    data_line = raw_data_line.decode('latin-1').strip()
                    print("Recibido:", data_line)
                except UnicodeDecodeError:
                    print("Error de decodificación:", raw_data_line)
                    continue
                
                match = data_pattern.match(data_line)
                if match:
                    index = int(match.group(1))
                    value = float(match.group(2))
                    data_values[index] = value
            
            # Escribir los datos en el archivo CSV
            csv_writer.writerow(data_values)
            csv_file.flush()  # Forzar la escritura inmediata
            
            print("Datos guardados:", data_values)
