# Hệ thống máy chấm công tích hợp mở cửa tự động

## Vai trò của tôi trong dự án
Tôi là người chịu trách nhiệm phát triển hệ thống toàn diện, bao gồm lập trình, triển khai phần cứng, và tích hợp dữ liệu với Google Sheets.

## Chi tiết về phần cứng và phần mềm

### Phần cứng
- Bo mạch **ESP32** với 30 chân để điều khiển các thiết bị trong hệ thống.
- **Module RFID MFRC522** để nhận diện thẻ từ.
- **Động cơ servo** để điều khiển mở/đóng cửa.
- **Cảm biến hồng ngoại** để đảm bảo an toàn khi cửa đang mở.
- Dữ liệu thẻ được lưu trữ trên **Google Sheets** để theo dõi trạng thái ra vào của nhân viên.

### Phần mềm
- Lập trình bằng **Arduino IDE** để điều khiển ESP32.
- Viết **Google Apps Script** để tương tác với Google Sheets. 
- Code Arduino được thiết kế để xử lý quét thẻ, điều khiển động cơ servo, và giám sát thời gian vào/ra theo yêu cầu dự án.

## Vì sao tôi làm dự án này, tính khả thi của dự án
Tôi thực hiện dự án này để giải quyết nhu cầu giám sát việc ra vào của nhân viên một cách tự động và chính xác hơn, giảm thiểu sai sót khi ghi chép thủ công. Tính khả thi của dự án cao vì việc sử dụng RFID và ESP32 là những công nghệ phổ biến, chi phí thấp nhưng hiệu quả cao. Việc tích hợp với Google Sheets cũng giúp dễ dàng quản lý và truy cập dữ liệu từ xa.

## Kinh nghiệm tích lũy từ dự án
Tôi đã học được cách tích hợp phần cứng và phần mềm để tạo ra một hệ thống hoàn chỉnh, từ lập trình ESP32 đến viết script cho Google Sheets. Tôi cũng hiểu rõ hơn về việc quản lý thời gian xử lý và đảm bảo tính ổn định cho hệ thống. Bên cạnh đó, việc làm việc với các cảm biến và module khác nhau giúp tôi cải thiện kỹ năng xử lý tín hiệu và tích hợp hệ thống an toàn.
