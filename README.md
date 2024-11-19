# Hệ thống máy chấm công tích hợp mở cửa tự động

## Mô tả hoạt động của hệ thống
Hệ thống hoạt động như sau: Khi một nhân viên quét thẻ RFID, UID của thẻ sẽ được kiểm tra với cơ sở dữ liệu trên Google Sheets. Nếu thẻ hợp lệ, cửa sẽ mở ra và trạng thái vào của nhân viên được cập nhật vào bảng Google Sheet. Sau khoảng thời gian cố định (5 giây), cửa sẽ tự động đóng lại. Nếu thẻ không hợp lệ, cửa sẽ không mở đồng thời phát loa cảnh báo và không có thay đổi nào trong hệ thống. Cảm biến hồng ngoại giúp phát hiện nếu có ai đứng giữa cửa, ngăn không cho cửa đóng lại cho đến khi lối đi thông thoáng.

## Vai trò trong dự án
Lập trình code C, viết Google App Script, triển khai phần cứng, và tích hợp dữ liệu với Google Sheets.

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

## Vì sao làm dự án này, tính khả thi của dự án
Môi trường làm việc hiện nay mỗi công ty, tổ chức đều phải có máy chấm công nhân viên để căn cứ vào đó trả mức lương tương đương với mức độ cống hiến và cũng để giải quyết nhu cầu giám sát việc ra vào của nhân viên một cách tự động và chính xác. Tính khả thi của dự án cao vì việc sử dụng RFID và ESP32 là những công nghệ phổ biến, chi phí thấp nhưng mang lại hiệu quả cao tương đương. Việc tích hợp với Google Sheets cũng giúp dễ dàng quản lý và truy cập dữ liệu từ xa.

## Kinh nghiệm tích lũy từ dự án
Tôi đã học được cách tích hợp phần cứng và phần mềm để tạo ra một hệ thống hoàn chỉnh, từ lập trình ESP32 đến viết Google appscript để tương tác gửi dữ liệu lên Google Sheets. Tôi cũng hiểu rõ hơn về việc quản lý thời gian xử lý và đảm bảo tính ổn định cho hệ thống. Bên cạnh đó, việc làm việc với các cảm biến và module khác nhau giúp tôi cải thiện kỹ năng xử lý và tích hợp hệ thống an toàn.

[Xem chi tiết báo cáo về dự án](https://drive.google.com/drive/folders/13U9VXYe0Kk4yexsRuw0DIJmfHk39w_uz?dmr=1&ec=wgc-drive-globalnav-goto)
