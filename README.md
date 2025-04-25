# QLVDT
# 📌 Giới thiệu dự án
Đây là hệ thống quản lý ví điểm thưởng, hỗ trợ đăng nhập/đăng ký, phân quyền người dùng, xác thực OTP nội bộ qua hệ thống mail giả lập.
# 👨‍💻 Thành viên tham gia dự án
K24DTCN249 - Mạc Quang Vinh : One For All :)
# 🛠 Phân tích và mô tả chức năng
- Chương trình chính (app.exe)  
Đăng ký tài khoản.  
Đăng nhập và phân quyền người dùng / quản lý.  
Chuyển điểm giữa các tài khoản.  
Quản lý ví điểm và lịch sử giao dịch.  
OTP(One Time Password) được tích hợp khi chuyển đổi điểm và người dùng thay đổi thông tin cá nhân qua chương trình mail giả lập.(**bắt buộc phải tạo và sử dụng mail tạo qua chương trình mail giả lập để nhận OTP(nên tạo mail qua chương trình mail giả lập trước khi đăng kí )**)  
Lưu trữ và backup dữ liệu  
- Chương trình phụ ( mail.exe - dùng để nhận mã OTP)  
Đăng kí , đăng nhập tài khoản mail ( mail phải có định dạng @gmail.com)  
Gửi thư  
Xem hôp thư đến  
# 🛠 Phân tích và mô tả chức năng theo phân quyền người dùng
1.User
Xem thông tin số dư
Thay đổi mật khẩu 
Thay đổi thông tin cá nhân
Chuyển điểm
Xem lịch sử chuyển đổi điểm 
Xem thông tin cá nhân của mình
2.Admin
Có tất cả chức năng của và :
Tạo tài khoản cho user
Đặt lại mật khẩu cho user
Tìm thông tin user
Xem tất cả user có trong hệ thống
# Hướng dẫn sử dụng
Chương trình đã biên dịch ra 2 file : app.exe và mail.exe có thể dùng được luôn hoặc có thể tự biên dịch chương trình : con trỏ vào thư mục chứa chương trình và dùng lệnh : g++ main.cpp user.cpp wallet.cpp send_internal_mail.cpp utils.cpp -o app.exe để dịch chương trình chính(app.exe) và : g++ main.cpp -o mail.exe để dịch chương trình phụ(mail.exe)
