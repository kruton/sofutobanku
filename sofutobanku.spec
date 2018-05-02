Summary:        Utility to setup Internet connection on Softbank Hikari
Name:           sofutobanku
Version:        1.0
Release:        2
License:        ASL 2.0
Group:          System Environment/Base
Vendor:         Kenny Root
Url:            https://github.com/kruton/sofutobanku/

Source:         sofutobanku-1.0.tar.gz

BuildRequires:  cmake >= 3.10
BuildRequires:  gtest-devel
BuildRequires:  nettle-devel >= 2.6
BuildRequires:  radcli-devel

%description
Sets up Internet connections for Softbank Hikari

%prep
%setup -q

%build
%cmake .
%make_build

%install
%make_install

%check
ctest -V %{?_smp_mflags}

%files
%defattr(755,root,root,-)
%{_sysconfdir}/sofutobanku/dibbler.sh
%{_sbindir}/sbradclient
%defattr(-,root,root,-)
%{_sysconfdir}/sofutobanku/dictionary.softbank

%changelog
* Mon Mar 5 2018 Kenny Root <kenny@the-b.org> 1.0.0-1
- First release.
