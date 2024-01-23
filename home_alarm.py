import RPi.GPIO as GPIO
import time
import datetime
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders
import cv2

# GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

# Define GPIO pins for the LED
GPIO_LED = 25

# Define GPIO pins for the ultrasonic sensor
GPIO_TRIGGER = 23
GPIO_ECHO = 24

# Set GPIO direction for LED
GPIO.setup(GPIO_LED, GPIO.OUT)

# Set GPIO direction for the trigger and echo pins
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)

def measure_distance():
    """
    Measures the distance using an HC-SR04 ultrasonic sensor.
    :return: Distance in centimeters.
    """
    # Trigger ultrasonic burst
    GPIO.output(GPIO_TRIGGER, True)
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)

    # Measure the duration of echo
    StartTime = time.time()
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()
    StopTime = time.time()
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()

    # Calculate distance
    TimeElapsed = StopTime - StartTime
    distance = (TimeElapsed * 34300) / 2
    return distance

def capture_image(camera):
    """
    Captures an image using the specified camera.
    :param camera: Camera object used for capturing the image.
    :return: Tuple of the image path and filename.
    """
    ret, frame = camera.read()
    if not ret:
        raise RuntimeError("Failed to capture image")

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    filename = f'{timestamp}.jpg'
    image_path = f'/home/everest/{filename}'

    cv2.imwrite(image_path, frame)
    return image_path, filename

def create_email(image_path, filename, timestamp):
    """
    Creates an email message with an image attached.
    :param image_path: Path to the image file.
    :param filename: Filename of the image.
    :param timestamp: Timestamp to include in the email subject.
    :return: The email message as a string.
    """
    sender_email = 'everest190124@gmail.com'
    receiver_email = 'giaan307@gmail.com'
    subject = f'Intruder Alert on {timestamp}'
    body = f'Intruder Alert! The following image was captured on: {timestamp}'

    # Set up MIME structure
    msg = MIMEMultipart()
    msg['From'] = sender_email
    msg['To'] = receiver_email
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))

    # Attach the image
    with open(image_path, 'rb') as attachment:
        part = MIMEBase('application', 'octet-stream')
        part.set_payload(attachment.read())
        encoders.encode_base64(part)
        part.add_header('Content-Disposition', f"attachment; filename= {filename}")
        msg.attach(part)

    return msg.as_string()

def send_email(message, receiver_email):
    """
    Sends an email message.
    :param message: The email message to send.
    :param receiver_email: The email address of the receiver.
    """
    sender_email = 'everest190124@gmail.com'
    sender_password = 'pfdw rkma btcj eovp'  # Store this securely

    # Log in and send the email
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    server.login(sender_email, sender_password)
    server.sendmail(sender_email, receiver_email, message)
    server.quit()

def main():
    """
    Main function to execute the program logic.
    """
    try:
        camera = cv2.VideoCapture(0)

        while True:
            dist = measure_distance()
            print(f"Measured Distance = {dist:.1f} cm")
            if dist < 10:  # Distance threshold in cm
                start_time = time.time()
                while (time.time() - start_time) < 10:  # Blink for 10 seconds
                    GPIO.output(GPIO_LED, GPIO.HIGH)  # LED on
                    time.sleep(0.3)  # On duration
                    GPIO.output(GPIO_LED, GPIO.LOW)  # LED off
                    time.sleep(0.3)  # Off duration
                    if (time.time() - start_time) > 5:  # Capture after 5 seconds of blinking
                        image_path, filename = capture_image(camera)
                        print(f"Image Captured: {filename}")
                        timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
                        email_message = create_email(image_path, filename, timestamp)
                        send_email(email_message, 'giaan307@gmail.com')
                        print("Email sent with attachment")
                        break  # Break the loop after sending the email

                GPIO.output(GPIO_LED, GPIO.LOW)  # Ensure LED is off after blinking
            else:
                GPIO.output(GPIO_LED, GPIO.LOW)  # Ensure LED is off if no motion is detected
            time.sleep(1)

    except RuntimeError as e:
        print(e)
    except KeyboardInterrupt:
        print("System stopped by user")
    finally:
        GPIO.cleanup()  # Clean up GPIO
        camera.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
