import pypdf

# Mở file PDF
reader = pypdf.PdfReader('LabSynchronization-2025.pdf')

# In tổng số trang
print(f"Tổng số trang: {len(reader.pages)}")

# Đọc văn bản từ trang đầu tiên (trang 0)
page1 = reader.pages[0]
print(page1.extract_text())

# Đọc văn bản toàn bộ tài liệu
for page_num, page in enumerate(reader.pages):
    print(f"--- Trang {page_num + 1} ---")
    print(page.extract_text())
