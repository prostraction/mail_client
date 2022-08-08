# mail_client
QT Mail Client

This branch created for rework of this project.
Roadmap:

1. Replace OpenSSL lib with ASIO lib and make it crosspatform. Replace any absolute paths. Move internetconnection sources to directory and make it indepent from QT.
2. Rework accoynt and authorize classes. Reject threading idea on SMTP. Move to own directory with all internet staff and make inpepent from QT.
3. Rework parser, make it inpepent from QT.
4. Rework MIME message staff, make it more stable to unknown MIME types.
5. Redraw forms.
6. Add sqlite lib.
7. Add google auth chromium staff.
